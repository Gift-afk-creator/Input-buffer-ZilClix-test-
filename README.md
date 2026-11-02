# ZilClix Input Buffer — Android build project

**Target:** Geometry Dash 2.2081 · Geode 5.11.2 · Android ARM64 (`arm64-v8a`).

## Important status

This package contains source code and a GitHub Actions build workflow. It is **not itself an installable `.geode` binary**. The workflow requests a cloud build for Android ARM64 and uploads the resulting `.geode` as a downloadable artifact if compilation succeeds.

The mod is currently a fixed-delay input queue prototype (1–16 update ticks). It is **not** the full automatic collision predictor shown in the video, and it has not been tested in-game. Fixed delay may make controls less responsive or alter timing. Test only in practice mode and keep a backup of your save.

## Build the Android `.geode` from a phone

1. Sign in to GitHub and create a **new empty repository**.
2. Upload the contents of this ZIP to the repository root. Make sure the `.github/workflows/android.yml` file is included (some mobile upload flows hide dot-folders; if the workflow does not appear under the repository's Actions tab, add that file through GitHub's web editor).
3. Open the repository's **Actions** tab.
4. Select **Build Android Geode mod** and tap **Run workflow** (or wait for the first push build).
5. If the build succeeds, open that workflow run, go to **Artifacts**, and download `ZilClix-InputBuffer-Android64`.
6. Extract the artifact. It should contain a file ending in `.geode`.

If the build fails, open the failed run and copy the first compiler error. Do not rename the ZIP to `.geode`.

## Install the compiled mod on Android

1. In a file manager, open:
   `Android/media/com.geode.launcher/game/geode/mods/`
2. Copy the actual `.geode` file into that `mods` folder.
3. Fully close and reopen Geometry Dash through Geode.
4. Open Geode's mod list and check whether **ZilClix Input Buffer** appears.
5. Keep the mod disabled until you are ready to test. Start in Practice Mode.

## Current settings

- `Enable input delay`: off by default.
- `Delay (frames)`: fixed delay of 1–16 game update ticks.

## What remains before calling it a recreation

A genuine automatic rescue system needs a tested predictor for the Android 2.2081 physics and collision pipeline, including gravity, speed, portals, slopes, game modes, dual player state and input timing. This prototype does not implement that predictor.
