# vgscript
UE plugin that enables external narrative scripts to be parsed, localized, and managed directly within the editor
VG Script  enables writers and designers to define dialogue, decisions, conditions, and actions in plain text, while the game dynamically interprets and executes them during runtime.
This plugin is currently in beta and under active development; core functionality is likely to evolve significantly as the system matures.

## Key Features
- **Script-Driven Narratives**: Define entire scenes and dialogue trees in readable text files.
- **Branching Logic**: Handle player choices, conditional paths, and dynamic actions using simple syntax.
- **Localization-Ready**: Every line of dialogue is assigned a unique key, allowing seamless localization via CSV.
- **Script Caching with DataTables**: VG Script parses and stores script content (text, decisions, conditions, actions) into Unreal DataTables, making them efficient to access at runtime.
- **UE Integration**: Works natively with Unreal Engine’s gameplay framework, allowing scripts to trigger gameplay events, animations, or other systems.

## Current Status
VGScript is in beta and experimental. Expect rough edges, and be aware that:
- Major systems may change or be refactored.
- Documentation is still in progress.
- Examples in the plugin Content in progress.
- Use in production is not yet recommended unless you’re comfortable with breaking changes.

## Who Is It For?
- Indie developers creating narrative-driven experiences.
- Writers and designers who prefer working with text-based scripts.
- Unreal Engine users who want a custom, extensible dialogue system without relying on heavy Blueprint graphs.
