
# TypeScript

[![GitHub Actions CI](https://github.com/microsoft/TypeScript/workflows/CI/badge.svg)](https://github.com/microsoft/TypeScript/actions?query=workflow%3ACI)
[![Devops Build Status](https://dev.azure.com/typescript/TypeScript/_apis/build/status/Typescript/node10)](https://dev.azure.com/typescript/TypeScript/_build?definitionId=7)
[![npm version](https://badge.fury.io/js/typescript.svg)](https://www.npmjs.com/package/typescript)
[![Downloads](https://img.shields.io/npm/dm/typescript.svg)](https://www.npmjs.com/package/typescript)

[TypeScript](https://www.typescriptlang.org/) is a language for application-scale JavaScript. TypeScript adds optional types to JavaScript that support tools for large-scale JavaScript applications for any browser, for any host, on any OS. TypeScript compiles to readable, standards-based JavaScript. Try it out at the [playground](https://www.typescriptlang.org/play/), and stay up to date via [our blog](https://blogs.msdn.microsoft.com/typescript) and [Twitter account](https://twitter.com/typescript).

Find others who are using TypeScript at [our community page](https://www.typescriptlang.org/community/).

## Installing

For the latest stable version:

```bash
npm install -g typescript
```

For our nightly builds:

```bash
npm install -g typescript@next
```

## Contribute

There are many ways to [contribute](https://github.com/microsoft/TypeScript/blob/main/CONTRIBUTING.md) to TypeScript.
* [Submit bugs](https://github.com/microsoft/TypeScript/issues) and help us verify fixes as they are checked in.
* Review the [source code changes](https://github.com/microsoft/TypeScript/pulls).
* Engage with other TypeScript users and developers on [StackOverflow](https://stackoverflow.com/questions/tagged/typescript).
* Help each other in the [TypeScript Community Discord](https://discord.gg/typescript).
* Join the [#typescript](https://twitter.com/search?q=%23TypeScript) discussion on Twitter.
* [Contribute bug fixes](https://github.com/microsoft/TypeScript/blob/main/CONTRIBUTING.md).
* Read the archived language specification ([docx](https://github.com/microsoft/TypeScript/blob/main/doc/TypeScript%20Language%20Specification%20-%20ARCHIVED.docx?raw=true),
 [pdf](https://github.com/microsoft/TypeScript/blob/main/doc/TypeScript%20Language%20Specification%20-%20ARCHIVED.pdf?raw=true), [md](https://github.com/microsoft/TypeScript/blob/main/doc/spec-ARCHIVED.md)).

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information see
the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com)
with any additional questions or comments.

## Documentation

*  [TypeScript in 5 minutes](https://www.typescriptlang.org/docs/handbook/typescript-in-5-minutes.html)
*  [Programming handbook](https://www.typescriptlang.org/docs/handbook/intro.html)
*  [Homepage](https://www.typescriptlang.org/)

## Roadmap

For details on our planned features and future direction please refer to our [roadmap](https://github.com/microsoft/TypeScript/wiki/Roadmap).

## Changes

We made the following code modifications to support the ETS development paradigm.

### October 2021

* [x] Support custom component `struct` syntax, and add `StructDeclaration` and `EtsComponentExpression` node type.

### November 2021

* [x] Support `gotoDefinition` for custom component name and arguments.
* [x] Support completion of lifecycle for `StructDeclaration` type.
* [x] Support `@Builder` decorator for function and method.

### January 2022

* [x] Optimize eTS language support.
* [x] Support `@BuilderParam` decorator and custom container lambda syntax.
* [x] Add jsDoc and displayParts info in `getCompletionAtPosition` interface.
* [x] Support eTS language ScriptKind and Extension.
* [x] Add `PropertyAccessExpressionConditionCheck` for eTS.

### February 2022

* [x] Support `@Styles` decorator and related syntax.
* [x] Add check for `struct` name contain reserved tag name.
* [x] Support `stateStyles` property.
* [x] Performance optimization for eTS completion.
* [x] Support `@Extend` and `@Styles` decorators for function.