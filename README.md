# Completion Duck

A small Windows Geode mod for Geometry Dash 2.2081 / Geode 5.10.1.

When the level-complete animation begins, the mod lowers only the level-song
channel. The normal completion jingle—including a replaced `endStart.ogg`—keeps
playing at its normal volume, so it sounds louder relative to the song.

Default fade shape:

- Level song remains at 30% volume.
- It smoothly fades down in 0.35 seconds.
- It remains quiet for 6.5 seconds.
- It smoothly fades back to the original volume over 1.25 seconds.
- If the level restarts or closes, the original volume is restored immediately
  so the lowered volume cannot leak into the next attempt or menu.

Both values appear in **Geode > Completion Duck > Settings**.

## Easiest build: GitHub Actions

This project includes an automatic Windows builder. You do not need to install
Visual Studio, LLVM, CMake, Ninja, or the Geode SDK locally.

1. Create an empty GitHub repository.
2. Extract this ZIP and open the `CompletionDuck` folder.
3. Upload everything inside that folder to the repository, including the
   `.github` folder.
4. Commit the files. GitHub will automatically start **Build Completion Duck**.
5. Open the repository's **Actions** tab, select the completed build, and
   download the `CompletionDuck-Windows` artifact.
6. Extract that artifact to obtain the installable `.geode` file.
7. Put the `.geode` file in the Geometry Dash `geode/mods` folder and restart
   Geometry Dash.

## Build on Windows

1. Install the current Geode developer tools and SDK.
2. Open a terminal in this folder.
3. Run `geode build`.
4. Copy the generated `connor.completion_duck.geode` file into the Geometry
   Dash `geode/mods` folder, or run `geode build --install`.

Official setup guide: https://docs.geode-sdk.org/getting-started/create-mod/

## Notes

- This mod does not replace or edit `endStart.ogg`.
- It changes only the background-music channel during completion.
- If 30% is still too loud, try 15–20% in the mod settings.
- All three timing stages can be adjusted independently in the mod settings.
