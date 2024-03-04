/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */



/**
 * @monitor function decorator implementation and supporting classes MonitorV3 and AsyncMonitorV3
 */

/**
 * @observe class and @track class property decorators
 * ObserveV3 core helper class to keep track of all the object -> UINode/elmtId 
 * and Monitor/watchId dependencies.
 */

class ObserveV3 {
  public static readonly SYMBOL_REFS = Symbol('__use_refs__');
  private static readonly SYMBOL_PROXY_GET_TARGET = Symbol("__proxy_get_target");

  public static readonly OB_PREFIX = "__ob_";  // OB_PREFIX + attrName => backing store attribute name
  public static readonly OB_PREFIX_LEN = 5;

  // used by array Handler to create dependency on artificial "length"
  // property of array, mark it as changed when array has changed.
  private static readonly OB_LENGTH = "length"; 
  private static readonly OB_DATE = "__date__";

  // see MonitorV3.observeObjectAccess: bindCmp is the MonitorV3
  // see modified observeComponentCreation, bindCmp is the ViewPU
  private bindCmp_: MonitorV3 | ViewPU | null = null;

  // bindId: UINode elmtId or watchId, depending on what is being observed
  private bindId_: number = -1;

  // Map bindId to ViewPU/MonitorV3
  // FIXME use Map<number, ViewPU | MonitorV3>
  private id2cmp_: { number: object } = {} as { number: object }

  // Map bindId -> Set 0f view model object
  // reverse dependency map for quickly removing all dependencies of a bindId
  // FIXME: string typing: Map<number, Set<Object>>
  private id2targets_: { number: object } = {} as { number: object }

  // queued up Set of bindId
  // elmtIds of UINodes need re-render
  // @monitor functions that need to execute
  private elmtIdsChanged_: Set<number> = new Set()
  private monitorIdsChanged_: Set<number> = new Set()

  // avoid recursive execution of updateDirty
  // by state changes => fireChange while
  // UINode rerender or @monitor function execution
  private startDirty_: boolean = false

  // flag to indicate change observation is disabled
  private disabled_ : boolean = false;

  private static obsInstance_: ObserveV3;

  public static getObserve(): ObserveV3 {
    if (!this.obsInstance_) {
      this.obsInstance_ = new ObserveV3();
    }
    return this.obsInstance_;
  }

  // At the start of observeComponentCreation or
  // MonitorV3.observeObjectAccess
  public startBind(cmp: ViewPU | MonitorV3 | null, id: number): void {
    this.bindCmp_ = cmp
    this.bindId_ = id;
    if (cmp != null) {
      this.clearBinding(id);
      this.id2cmp_[id] = cmp;
    }
  }

  // clear any previously created dependency view model object to elmtId
  // find these view model objects with the reverse map id2targets_
  private clearBinding(id: number): void {
    this.id2targets_[id]?.forEach((target) => {
      for (let key in target[ObserveV3.SYMBOL_REFS]) {
        if (id in target[ObserveV3.SYMBOL_REFS][key]) {
          delete target[ObserveV3.SYMBOL_REFS][key][id]
        }
      }
    })
    delete this.id2targets_[id]
    delete this.id2cmp_[id]
  }

  // add dependency view model object 'target' property 'attrName'
  // to current this.bindId
  public addRef(target: object, attrName: string): void {
    if (this.bindCmp_ === null) {
      return
    }

    if (!target[ObserveV3.SYMBOL_REFS]) {
      target[ObserveV3.SYMBOL_REFS] = {}
    }
    if (!target[ObserveV3.SYMBOL_REFS][attrName]) {
      target[ObserveV3.SYMBOL_REFS][attrName] = {}
    }

    let obj = target[ObserveV3.SYMBOL_REFS][attrName];
    obj[this.bindId_] = 1

    if (!this.id2targets_[this.bindId_]) {
      this.id2targets_[this.bindId_] = new Set();
    }
    this.id2targets_[this.bindId_].add(target);
  }

  /**
   * setReadOnlyAttr - helper function used to update an immutable attribute
   * such update as a @param variable from parent @Component
   * @param target  - the object, usually the ViewPU
   * @param attrName - @param variable name
   * @param newValue - update to new value
   */
  public setReadOnlyAttr<Z>(target: object, attrName: string, newValue: Z): void {
    const storeProp = ObserveV3.OB_PREFIX + attrName;
    if (storeProp in target) {
      // @observed class and @track attrName
      if (newValue === target[storeProp]) {
        stateMgmtConsole.propertyAccess(`setReadOnlyAttr '${attrName}' - tracked but unchanged. Doing nothing.`);
        return;
      }
      stateMgmtConsole.propertyAccess(`setReadOnlyAttr '${attrName}' - updating backing store and fireChange.`);
      target[storeProp] = newValue;
      ObserveV3.getObserve().fireChange(target, attrName)
    } else {
      stateMgmtConsole.propertyAccess(`setReadOnlyAttr '${attrName}' - untracked, assigning straight.`);
      // untracked attrName
      target[attrName] = newValue;
    }
  }

  /**
   * 
   * @param target set tracked attribute to new value without notifying the change
   *               !! use with caution !!
   * @param attrName 
   * @param newValue 
   */
  public setUnmonitored<Z>(target: object, attrName: string, newValue: Z): void {    
    const storeProp = ObserveV3.OB_PREFIX + attrName;
    if (storeProp in target) {
      // @track attrName
      stateMgmtConsole.propertyAccess(`setUnmonitored '${attrName}' - tracked but unchanged. Doing nothing.`);
      target[storeProp] = newValue;
    } else {
      stateMgmtConsole.propertyAccess(`setUnmonitored '${attrName}' - untracked, assigning straight.`);
      // untracked attrName
      target[attrName] = newValue;
    }
  }

  /**
   * Execute given task while state change observation is disabled
   * A state mutation caused by the task will NOT trigger UI rerender 
   * and @monitor function execution. 
   * 
   * !!! Use with Caution !!!
   * 
   * @param task 
   * @returns 
   */
  public executeUnobserved<Z>(task: () => Z): Z {
    stateMgmtConsole.propertyAccess(`executeUnobserved - start`);
    this.disabled_ = true;
    let ret : Z;
    try {
      ret = task();
    } catch (e) {
      stateMgmtConsole.applicationError(`executeUnobserved - task execution caused error ${e} !`);
    }
    this.disabled_ = false;
    stateMgmtConsole.propertyAccess(`executeUnobserved - done`);
    return ret;
  }
 

  // mark view model object 'target' property 'attrName' as changed
  // notify affected watchIds and elmtIds
  public fireChange(target: object, attrName: string): void {
    if (!target[ObserveV3.SYMBOL_REFS] || this.disabled_) {
      return;
    }
    let obj = target[ObserveV3.SYMBOL_REFS][attrName];
    if (!obj) {
      return;
    }

    stateMgmtConsole.propertyAccess(`ObserveV3.fireChange ...`);

    // FIXME seem to cause the crash, investigate
    //  obj.forEach((id : number) => {
    for (let idA in obj) {
      const id = parseInt(idA);

      // Cannot fireChange the object that is being created.
      if (id === this.bindId_) {
        return
      }

      // if this is the first id to be added to elmtIdsChanged_ and monitorIdsChanged_, 
      // schedule an 'updateDirty' task
      // that will run after the current call stack has unwound.
      // purpose of check for startDirty_ is to avoid going into recursion. This could happen if
      // exec a re-render or exec a monitor function changes some state -> calls fireChange -> ...
      if ((0 === this.elmtIdsChanged_.size) && (0 === this.monitorIdsChanged_.size)
        && !this.startDirty_) {
        Promise.resolve(true).then(this.updateDirty.bind(this))
      }
      // add bindId to Set of pending changes.
      (id < MonitorV3.MIN_WATCH_ID)
        ? this.elmtIdsChanged_.add(id)
        : this.monitorIdsChanged_.add(id);
    } // for
  }

  private updateDirty(): void {
    this.startDirty_ = true
    this.updateDirty2()
    this.startDirty_ = false
  }

  private updateDirty2(): void {
    // process monitors first, because these might add more elmtIds of UINodes to rerender
    this.updateDirtyMonitors(1);
    this.notifyDirtyElmtIdsToOwningViews();
  }

  private updateDirtyMonitors(recursionDepth : number): void {
    if (recursionDepth>20) {
      // limit recursion depth to avoid infinite loops
      stateMgmtConsole.applicationError(`20 loops in @monitor function execution detected. Stopping processing. Application error!`)
      this.monitorIdsChanged_ = new Set<number>();
      return;
    }
    stateMgmtConsole.debug(`updateDirtyMonitors  ${JSON.stringify(Array.from(this.monitorIdsChanged_))} ...`);
    const monitors = this.monitorIdsChanged_; // move Set
    // exec @monitor functions might add new watchIds
    this.monitorIdsChanged_ = new Set<number>();
    let monitor : MonitorV3 | undefined;
    monitors.forEach((watchId) => {
      if ((monitor = this.id2cmp_[watchId]) && (monitor instanceof MonitorV3)) {
        monitor.fireChange();
      }
    });

    if (this.monitorIdsChanged_.size) {
      this.updateDirtyMonitors(recursionDepth+1);
    }
  }

  private notifyDirtyElmtIdsToOwningViews() : void {
    let view : ViewPU | undefined;
    stateMgmtConsole.debug(`notifyDirtyElmtIdsToOwningViews ${JSON.stringify(Array.from(this.elmtIdsChanged_))} ...`);
    this.elmtIdsChanged_.forEach((elmtId) => {
      if ((view = this.id2cmp_[elmtId]) && (view instanceof ViewPU)) {
        // FIXME uiNodeNeedUpdateV3 just copies elmtIfs to another set
        // waits for FlushBuild to call rerender call updateDirtyElements
        // to actually render the UINodes. Could we call ViewPU.UpdateElement 
        // right away?        
        view.uiNodeNeedUpdateV3(elmtId);
      }
    });
    this.elmtIdsChanged_.clear();
  }

  public constructMonitor(target: any, name: string) : void {
    let watchProp = Symbol.for(MonitorV3.WATCH_PREFIX + name)
    if (target && target[watchProp]) {
      Object.entries(target[watchProp]).forEach(([key, val]) => {
        ObserveV3.getObserve().addWatch(target, key, val)
      })
    }
  }


  public addWatch(target, props, func): number {
    return new MonitorV3(target, props, func).InitRun()
  }

  public clearWatch(id: number): void {
    this.clearBinding(id)
  }


  public static autoProxyObject(target : Object, key : string | symbol)  : any {
    let val = target[key]
    // Not an object, not a collection, no proxy required
    if (!val || typeof (val) !== "object" || !(Array.isArray(val) ||
        val instanceof Set || val instanceof Map || val instanceof Date)) {
      return val
    }

    // Only collections require proxy observation, and if it has been observed, it does not need to be observed again.
    if (!val[ObserveV3.SYMBOL_PROXY_GET_TARGET]) {
      target[key] = new Proxy(val, this.arraySetMapProxy)
      val = target[key]
    }

    // If the return value is an array, a length observation should be added to the array.
    if (Array.isArray(val)) {
      ObserveV3.getObserve().addRef(val, this.OB_LENGTH)
    }

    return val
  }

  private static readonly arrayLengthChangingFunctions = new Set(["push", "pop", "shift", "splice", "unshift"]);
  private static readonly arrayMutatingFunctions = new Set(["copyWithin", "fill", "reverse", "sort"]);
  private static readonly dateSetFunctions = new Set(["setFullYear", "setMonth", "setDate", "setHours", "setMinutes",
    "setSeconds", "setMilliseconds", "setTime", "setUTCFullYear", "setUTCMonth", "setUTCDate", "setUTCHours",
    "setUTCMinutes", "setUTCSeconds", "setUTCMilliseconds"]);

  public static readonly arraySetMapProxy = {
    get(target: any, key: string | symbol, receiver: any) {
      if (typeof key === "symbol") {
        return key === ObserveV3.SYMBOL_PROXY_GET_TARGET ? target : target[key]
      }
      let ret = ObserveV3.autoProxyObject(target, key)
      if (typeof (ret) !== "function") {
        ObserveV3.getObserve().addRef(target, key)
        return ret
      }

      if (Array.isArray(target)) {
        if (ObserveV3.arrayMutatingFunctions.has(key)) {
          return function (...args) {
            ret.call(target, ...args);
            ObserveV3.getObserve().fireChange(target, ObserveV3.OB_LENGTH);
            // returning the 'receiver(proxied object)' ensures that when chain calls also 2nd function call
            // operates on the proxied object.
            return receiver;
          };
        } else if (ObserveV3.arrayLengthChangingFunctions.has(key)) {
          return function (...args) {
            const result = ret.call(target, ...args);
            ObserveV3.getObserve().fireChange(target, ObserveV3.OB_LENGTH);
            return result;
          };
        } else { 
          return ret.bind(receiver);
        }
      }

      if (target instanceof Date) {
        if (ObserveV3.dateSetFunctions.has(key)) {
          return function (...args) {
            // execute original function with given arguments
            let result = ret.call(this, ...args);
            ObserveV3.getObserve().fireChange(target, ObserveV3.OB_DATE);
            return result;
            // bind "this" to target inside the function
          }.bind(target)
        } else {
            ObserveV3.getObserve().addRef(target, ObserveV3.OB_DATE);
        }
        return ret.bind(target);
      }

      if (target instanceof Set || target instanceof Map) {
        if (key === "has") {
          return prop => {
            ObserveV3.getObserve().addRef(target, prop)
            return target.has(prop)
          }
        }
        if (key === "delete") {
          return prop => {
            ObserveV3.getObserve().fireChange(target, prop)
            if (target.has(prop)) {
              ObserveV3.getObserve().fireChange(target, this.OB_LENGTH)
            }
            return target.delete(prop)
          }
        }
        if (key === "clear") {
          return () => {
            target.forEach((_, prop) => {
              ObserveV3.getObserve().fireChange(target, prop.toString())
            })
            if (target.size > 0) {
              ObserveV3.getObserve().fireChange(target, this.OB_LENGTH)
            }
            target.clear()
          }
        }
        if (key === "keys" || key === "values" || key === "entries") {
          return () => {
            ObserveV3.getObserve().addRef(target, this.OB_LENGTH)
            return target[key]()
          }
        }
      }

      if (target instanceof Set) {
        return key === "add" ? val => {
          ObserveV3.getObserve().fireChange(target, val.toString())
          if (!target.has(val)) {
            ObserveV3.getObserve().fireChange(target, this.OB_LENGTH)
          }
          return target.add(val)
        } : ret
      }

      if (target instanceof Map) {
        if (key === "get") { // for Map
          return (prop) => {
            ObserveV3.getObserve().addRef(target, prop)
            if (!target.has(prop)) {
              ObserveV3.getObserve().fireChange(target, this.OB_LENGTH)
            }
            return target.get(prop)
          }
        }
        if (key === "set") { // for Map
          return (prop, val) => {
            ObserveV3.getObserve().fireChange(target, prop)
            if (!target.has(prop)) {
              ObserveV3.getObserve().fireChange(target, this.OB_LENGTH)
            }
            return target.set(prop, val)
          }
        }
      }

      return ret
    },
    set(target: any, key: string | symbol, value: any) {
      if (typeof key === 'symbol') {
        if (key !== ObserveV3.SYMBOL_PROXY_GET_TARGET) {
          target[key] = value
        }
        return true;
      }

      if (target[key] === value) {
        return true;
      }
      target[key] = value
      ObserveV3.getObserve().fireChange(target, key.toString())
      return true
    }
  }
}

/**
 * @track class property decorator 
 * 
 * @param target  class prototype object
 * @param propertyKey  class property name
 * 
 * turns given property into getter and setter functions
 * adds property target[storeProp] as the backing store
 * 
 * part of SDK
 * @from 12
 */
const track = (target: Object, propertyKey: string) => {
  ConfigureStateMgmt.instance.intentUsingV3(`@track`, propertyKey);
  return trackInternal(target, propertyKey);
}

const trackInternal = (target: any, propertyKey: string) => {
  if (typeof target === "function" && !Reflect.has(target, propertyKey)) {
    // dynamic track，and it not a static attribute
    target = target.prototype
  }
  let storeProp = ObserveV3.OB_PREFIX + propertyKey
  target[storeProp] = target[propertyKey]
  Reflect.defineProperty(target, propertyKey, {
    get() {
      ObserveV3.getObserve().addRef(this, propertyKey)
      return ObserveV3.autoProxyObject(this, ObserveV3.OB_PREFIX + propertyKey)
    },
    set(val) {
      // If the object has not been observed, you can directly assign a value to it. This improves performance.
      if (val !== this[storeProp]) {
        this[storeProp] = val
      if (this[ObserveV3.SYMBOL_REFS]) { // This condition can improve performance.
          ObserveV3.getObserve().fireChange(this, propertyKey)
        }
      }
    },
    enumerable: true
  })
} // track


/**
 * @observe class decorator
 * only @observe classes can have functional @track attributes inside.
 * and only changes of such decorated properties can be deep observed
 * (decorated along the entire path from root object to property)
 * 
 * part of SDK
 * @from 12
 * 
 */
type Constructor = { new(...args: any[]): any };

function observed<T extends Constructor>(BaseClass: T) : Constructor {
  ConfigureStateMgmt.instance.intentUsingV3(`@observed`, BaseClass.name);
  return class extends BaseClass {
    constructor(...args) {
      super(...args)
      // After a "new" object, no matter how many times the watched value is assigned,
      // only the last initial value is recognized. Therefore, you need to add "Monitor" asynchronously.
      // Promise.resolve(true).then(() => constructMonitor(this, BaseClass.name)) // Low performance
      AsyncAddMonitorV3.addWatch(this, BaseClass.name)
    }
  }
}

