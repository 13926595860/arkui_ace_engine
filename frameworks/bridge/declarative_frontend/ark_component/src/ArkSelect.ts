/// <reference path='./import.ts' />
class ArkSelectComponent extends ArkComponent implements SelectAttribute {
  constructor(nativePtr: KNode) {
    super(nativePtr);
  }
  onGestureJudgeBegin(callback: (gestureInfo: GestureInfo, event: BaseGestureEvent) => GestureJudgeResult): this {
    throw new Error('Method not implemented.');
  }
  optionWidth(value: Dimension | OptionWidthMode): this {
    throw new Error('Method not implemented.');
  }
  optionHeight(value: Dimension): this {
    throw new Error('Method not implemented.');
  }
  selected(value: number | Resource): this {
    modifierWithKey(
      this._modifiersWithKeys, SelectedModifier.identity, SelectedModifier, value);
    return this;
  }
  value(value: ResourceStr): this {
    modifierWithKey(
      this._modifiersWithKeys, ValueModifier.identity, ValueModifier, value);
    return this;
  }
  font(value: Font): this {
    modifierWithKey(
      this._modifiersWithKeys, FontModifier.identity, FontModifier, value);
    return this;
  }
  fontColor(value: ResourceColor): this {
    modifierWithKey(
      this._modifiersWithKeys, SelectFontColorModifier.identity, SelectFontColorModifier, value);
    return this;
  }
  selectedOptionBgColor(value: ResourceColor): this {
    modifierWithKey(
      this._modifiersWithKeys, SelectedOptionBgColorModifier.identity, SelectedOptionBgColorModifier, value);
    return this;
  }
  selectedOptionFont(value: Font): this {
    modifierWithKey(
      this._modifiersWithKeys, SelectedOptionFontModifier.identity, SelectedOptionFontModifier, value);
    return this;
  }
  selectedOptionFontColor(value: ResourceColor): this {
    modifierWithKey(
      this._modifiersWithKeys, SelectedOptionFontColorModifier.identity, SelectedOptionFontColorModifier, value);
    return this;
  }
  optionBgColor(value: ResourceColor): this {
    modifierWithKey(
      this._modifiersWithKeys, OptionBgColorModifier.identity, OptionBgColorModifier, value);
    return this;
  }
  optionFont(value: Font): this {
    modifierWithKey(
      this._modifiersWithKeys, OptionFontModifier.identity, OptionFontModifier, value);
    return this;
  }
  optionFontColor(value: ResourceColor): this {
    modifierWithKey(
      this._modifiersWithKeys, OptionFontColorModifier.identity, OptionFontColorModifier, value);
    return this;
  }
  onSelect(callback: (index: number, value: string) => void): this {
    throw new Error('Method not implemented.');
  }
  space(value: Length): this {
    modifierWithKey(
      this._modifiersWithKeys, SpaceModifier.identity, SpaceModifier, value);
    return this;
  }
  arrowPosition(value: ArrowPosition): this {
    modifierWithKey(
      this._modifiersWithKeys, ArrowPositionModifier.identity, ArrowPositionModifier, value);
    return this;
  }
  menuAlign(alignType: MenuAlignType, offset?: Offset): this {
    let menuAlign = new ArkMenuAlignType(alignType, offset);
    modifierWithKey(
      this._modifiersWithKeys, MenuAlignModifier.identity, MenuAlignModifier, menuAlign);
    return this;
  }
}

class FontModifier extends ModifierWithKey<Font> {
  constructor(value: Font) {
    super(value);
  }
  static identity: Symbol = Symbol('selectFont');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      GetUINativeModule().select.resetFont(node);
    }
    else {
      GetUINativeModule().select.setFont(node, this.value.size, this.value.weight, this.value.family, this.value.style);
    }
  }

  checkObjectDiff(): boolean {
    let sizeEQ = isBaseOrResourceEqual(this.stageValue.size, this.value.size);
    let weightEQ = this.stageValue.weight === this.value.weight;
    let familyEQ = isBaseOrResourceEqual(this.stageValue.family, this.value.family);
    let styleEQ = this.stageValue.style === this.value.style;
    return !sizeEQ || !weightEQ || !familyEQ || !styleEQ;
  }
}

class OptionFontModifier extends ModifierWithKey<Font> {
  constructor(value: Font) {
    super(value);
  }
  static identity: Symbol = Symbol('selectOptionFont');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      GetUINativeModule().select.resetOptionFont(node);
    }
    else {
      GetUINativeModule().select.setOptionFont(node, this.value.size, this.value.weight, this.value.family, this.value.style);
    }
  }

  checkObjectDiff(): boolean {
    let sizeEQ = isBaseOrResourceEqual(this.stageValue.size, this.value.size);
    let weightEQ = this.stageValue.weight === this.value.weight;
    let familyEQ = isBaseOrResourceEqual(this.stageValue.family, this.value.family);
    let styleEQ = this.stageValue.style === this.value.style;
    return !sizeEQ || !weightEQ || !familyEQ || !styleEQ;
  }
}

class SelectedOptionFontModifier extends ModifierWithKey<Font>{
  constructor(value: Font) {
    super(value);
  }
  static identity: Symbol = Symbol('selectSelectedOptionFont');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      GetUINativeModule().select.resetSelectedOptionFont(node);
    }
    else {
      GetUINativeModule().select.setSelectedOptionFont(node, this.value.size, this.value.weight, this.value.family, this.value.style);
    }
  }

  checkObjectDiff(): boolean {
    let sizeEQ = isBaseOrResourceEqual(this.stageValue.size, this.value.size);
    let weightEQ = this.stageValue.weight === this.value.weight;
    let familyEQ = isBaseOrResourceEqual(this.stageValue.family, this.value.family);
    let styleEQ = this.stageValue.style === this.value.style;
    return !sizeEQ || !weightEQ || !familyEQ || !styleEQ;
  }
}

class MenuAlignModifier extends ModifierWithKey<ArkMenuAlignType>{
  constructor(value: ArkMenuAlignType) {
    super(value);
  }
  static identity: Symbol = Symbol('selectMenuAlign');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      GetUINativeModule().select.resetMenuAlign(node);
    }
    else {
      GetUINativeModule().select.setMenuAlign(node, this.value.alignType, this.value.dx, this.value.dy);
    }
  }

  checkObjectDiff(): boolean {
    let alignTypeEQ = this.stageValue.alignType === this.value.alignType;
    let dxEQ = isBaseOrResourceEqual(this.stageValue, this.value);
    let dyEQ = isBaseOrResourceEqual(this.stageValue, this.value);

    return !alignTypeEQ || !dxEQ || !dyEQ;
  }

  private isEqual(stageValue: Length, value: Length) {
    if ((!isUndefined(stageValue) && isResource(stageValue)) &&
      (!isUndefined(value) && isResource(value))) {
      return !isResourceEqual(stageValue, value);
    } else {
      return stageValue !== value;
    }
  }
}

class ArrowPositionModifier extends ModifierWithKey<ArrowPosition>{
  constructor(value: ArrowPosition) {
    super(value);
  }
  static identity: Symbol = Symbol('selectArrowPosition');
  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      GetUINativeModule().select.resetArrowPosition(node);
    }
    else {
      GetUINativeModule().select.setArrowPosition(node, this.value);
    }
  }

  checkObjectDiff(): boolean {
    return this.stageValue !== this.value;
  }
}
class SpaceModifier extends ModifierWithKey<Length>{
  constructor(value: Length) {
    super(value);
  }
  static identity: Symbol = Symbol('selectSpace');

  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      GetUINativeModule().select.resetSpace(node);
    }
    else {
      GetUINativeModule().select.setSpace(node, this.value);
    }
  }

  checkObjectDiff(): boolean {
    return !isBaseOrResourceEqual(this.stageValue, this.value);
  }
}
class ValueModifier extends ModifierWithKey<ResourceStr> {
  constructor(value: ResourceStr) {
    super(value);
  }
  static identity: Symbol = Symbol('selectValue');

  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      GetUINativeModule().select.resetValue(node);
    }
    else {
      GetUINativeModule().select.setValue(node, this.value);
    }
  }

  checkObjectDiff(): boolean {
    return !isBaseOrResourceEqual(this.stageValue, this.value);
  }
}
class SelectedModifier extends ModifierWithKey<number | Resource> {
  constructor(value: number | Resource) {
    super(value);
  }
  static identity: Symbol = Symbol('selectSelected');

  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      GetUINativeModule().select.resetSelected(node);
    }
    else {
      GetUINativeModule().select.setSelected(node, this.value);
    }
  }

  checkObjectDiff(): boolean {
    return !isBaseOrResourceEqual(this.stageValue, this.value);
  }
}
class SelectFontColorModifier extends ModifierWithKey<ResourceColor> {
  constructor(value: ResourceColor) {
    super(value);
  }
  static identity: Symbol = Symbol('selectFontColor');

  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      GetUINativeModule().select.resetFontColor(node);
    }
    else {
      GetUINativeModule().select.setFontColor(node, this.value);
    }
  }
  checkObjectDiff(): boolean {
    return !isBaseOrResourceEqual(this.stageValue, this.value);
  }
}
class SelectedOptionBgColorModifier extends ModifierWithKey<ResourceColor> {
  constructor(value: ResourceColor) {
    super(value);
  }
  static identity: Symbol = Symbol('selectSelectedOptionBgColor');

  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      GetUINativeModule().select.resetSelectedOptionBgColor(node);
    }
    else {
      GetUINativeModule().select.setSelectedOptionBgColor(node, this.value);
    }
  }
  checkObjectDiff(): boolean {
    return !isBaseOrResourceEqual(this.stageValue, this.value);
  }
}
class OptionBgColorModifier extends ModifierWithKey<ResourceColor> {
  constructor(value: ResourceColor) {
    super(value);
  }
  static identity: Symbol = Symbol('selectOptionBgColor');

  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      GetUINativeModule().select.resetOptionBgColor(node);
    }
    else {
      GetUINativeModule().select.setOptionBgColor(node, this.value);
    }
  }
  checkObjectDiff(): boolean {
    return !isBaseOrResourceEqual(this.stageValue, this.value);
  }
}
class OptionFontColorModifier extends ModifierWithKey<ResourceColor>{
  constructor(value: ResourceColor) {
    super(value);
  }
  static identity: Symbol = Symbol('selectOptionFontColor');

  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      GetUINativeModule().select.resetOptionFontColor(node);
    }
    else {
      GetUINativeModule().select.setOptionFontColor(node, this.value);
    }
  }
  checkObjectDiff(): boolean {
    return !isBaseOrResourceEqual(this.stageValue, this.value);
  }
}
class SelectedOptionFontColorModifier extends ModifierWithKey<ResourceColor>{
  constructor(value: ResourceColor) {
    super(value);
  }
  static identity: Symbol = Symbol('selectSelectedOptionFontColor');

  applyPeer(node: KNode, reset: boolean): void {
    if (reset) {
      GetUINativeModule().select.resetSelectedOptionFontColor(node);
    }
    else {
      GetUINativeModule().select.setSelectedOptionFontColor(node, this.value);
    }
  }
  checkObjectDiff(): boolean {
    return !isBaseOrResourceEqual(this.stageValue, this.value);
  }
}

// @ts-ignore
globalThis.Select.attributeModifier = function (modifier) {
  const elmtId = ViewStackProcessor.GetElmtIdToAccountFor();
  let nativeNode = GetUINativeModule().getFrameNodeById(elmtId);
  let component = this.createOrGetNode(elmtId, () => {
    return new ArkSelectComponent(nativeNode);
  });
  modifier.applyNormalAttribute(component);
  component.applyModifierPatch();
};
