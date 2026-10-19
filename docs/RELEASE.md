# Releasing

The steps for a release, v0.1.0-alpha first. The version comes from the tag (`git describe --tags`): the menu's
footer, log.txt, the Mac app's Info.plist, the Windows exe's details and the site all read it, and the site's
build stops if a package isn't that version's build.

## 1. The code

1. Merge the work into `main` (the default branch) and push.
2. Tag `main` and push the tag:
   ```
   git tag -a v0.1.0-alpha -m "Virtua Fighter 3 Recomp v0.1.0-alpha" && git push origin v0.1.0-alpha
   ```

## 2. The downloads (from the tag, a clean checkout)

```
tools/mac/package.sh                                 # build/release/VirtuaFighter3Recomp-mac.zip (on a Mac)
tools/win/build.sh                                   # build/win/VirtuaFighter3Recomp-windows.zip
tools/deck/build.sh                                  # build/deck/VirtuaFighter3Recomp-deck.tar.gz
```

Check each opens and plays, and that the menu's footer says `v0.1.0-alpha` (not a commit).

## 3. The GitHub release

New release from the tag, titled `Virtua Fighter 3 Recomp v0.1.0-alpha`, with the three packages attached under
exactly those names. **Leave "Set as a pre-release" off:** the site's buttons link to
`releases/latest/download/...`, and GitHub's "latest" skips pre-releases, so they would 404. (The version's
`-alpha` says what it is.)

Then make the repository public (Settings > General > Danger Zone > Change visibility): the site, the README and
the YouTube post link to it.

## 4. The site (vf3-recomp.pages.dev, Cloudflare Pages)

```
tools/site/media.py "Virtua Fighter 3 Recomp - trailer (1080p60).mp4"   # once: the cabinet's screen, the poster
tools/site/build.py --youtube <the trailer's video ID>                    # build/site, checked against the tag
npx wrangler pages deploy build/site --project-name vf3-recomp --branch main
```

`build.py` needs the three packages from step 2 in `build/release`, `build/win` and `build/deck` (it only checks
they carry the version; the files themselves are on GitHub). The referrer count works as VF1's: a `.refhit-url`
at the repo root, and the `REF_KEY` secret on the Pages project.

## 5. After

- The VF1 and VF2 sites' More ports menus: Virtua Fighter 3 linked to https://vf3-recomp.pages.dev/.
- The README: the trailer's link.
