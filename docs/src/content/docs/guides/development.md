---
title: Development Guide
description: General guidelines about developing with ACE Anvil.
---

Here are some general guidelines about developing with ACE Anvil.

## Requirements

- Arma Reforger and Arma Reforger Tools (available on Steam)
- Git

## Development Environment

1. Clone [ACE Anvil](https://github.com/acemod/ACE-Anvil) with Git
1. Open Arma Reforger Tools -> Add Project -> Scan for Projects
1. Select `ACE-Anvil/addons` folder
1. Launch `ACE All in One`

Recommended Worlds to test on:
- `ArmaReforger/worlds/MP/MpTest/MpTest.ent`
- `ArmaReforger/worlds/ScenarioFramework/SF-Tutorial-Navmesh.ent` (if AI is required)

## Code Formatting

We will follow [Bohemia Interactive's guidelines](https://community.bistudio.com/wiki/Arma_Reforger:Scripting:_Conventions) for now.

## Debug Mode

All ACE components should have the following attribute to enable a debugging mode, if there is one:

```csharp
[Attribute(defvalue: "0", desc: "Enable debug mode")]
bool m_bDebugModeEnabled;
```


## Further reading

- Read official [scripting guidelines](https://community.bistudio.com/wiki/Category:Arma_Reforger/Modding/Guidelines/Scripting)
- Read official [scripting tutorials](https://community.bistudio.com/wiki/Category:Arma_Reforger/Modding/Tutorials/Scripting)
