# Comparatif SDMMC1, fils 10 cm — preuves bornées

Mission `TR2-20261014-P12H3H-SDMMC1-CABLE10CM-COMPARISON`.
Aucune écriture ni cycle : No Media initial persiste après un reset logiciel autorisé.

- `campaign-summary.json` : décision, nombre de cycles, resets et flashs.
- `initial-context.json` et `image-verification.json` : provenance et readback byte-identique ; image conservée localement, non redistribuée.
- `host-before-debug.json` : No Media observé **avant** le premier debug de cette mission.
- `initial-identity-image-state.*` : UID/CID/capacité/CLKCR/RCC/erreurs, dump Flash uniquement.
- `initial-no-media-recovery.*` : état avant et unique reset logiciel.
- `after-initial-recovery-preflight.*` : UID/CID/4 bits/12 MHz et erreur 0x2005 après reset.
- `host-after-initial-recovery.json` : No Media final, USB natif présent.
- `comparison.py`/`hardware.py` : commandes réelles et garde identité ; invocation `python3 comparison.py initial` puis `python3 comparison.py recover`. Chaque preuve porte un nom unique, pas de réutilisation destructive. Ne pas relancer après le critère d'arrêt.
- `NativeAligned.cs` : byte-identique à l'outil jeu B précédent ; **non exécuté dans cette mission** puisque le média n'est pas disponible.
- `manifest.json` : SHA-256 de tous les fichiers de ce dossier sauf lui-même.

Les sessions GDB halt/detach peuvent perturber le firmware USB/polling. No Media préexistait avant la première session ; la cause initiale n'est pas isolée pour autant. Les resets ne représentent pas une coupure électrique. Pas de conclusion sur la carte, le contrôleur ou une causalité certaine de la longueur des fils.
