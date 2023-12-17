# Development

Here are some general guidelines about developing with ACE Anvil.


## Development Environment

The `dev` folder contains an additional project with a prebuilt & playable world, which can be used for testing. At the point of writing, it contains no AI and player loadouts are randomized. This world should be expanded in the future.


## Code Formatting

We will follow [Bohemia Interactive's guidelines](https://community.bistudio.com/wiki/Arma_Reforger:Scripting:_Conventions) for now.


## Debug Mode

All ACE components should have the following attribute to enable a debugging mode, if there is one:

```cs
[Attribute(defvalue: "0", desc: "Enable debug mode")]
bool m_bDebugModeEnabled;
```
