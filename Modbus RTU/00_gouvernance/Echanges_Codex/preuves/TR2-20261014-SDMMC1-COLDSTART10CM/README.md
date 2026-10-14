# SDMMC1 COLDSTART10CM — preuves minimales

- `host-before-debug.json` : observation Windows avant tout GDB/reset, No Media.
- `initial-identity-image-state.*` : UID, CID du handle, configuration, erreurs et readback Flash ; aucune commande SD, aucun reset.
- `image-verification.json` : hash et comparaison complète des31437octets de l'image, pas un hash de toute la Flash.
- `host-after-initial-debug.json` : état final Windows No Media.
- `campaign-summary.json` : zéro cycle/écriture/reset/flash, décision FAILED et limites.
- `comparison.py` + `hardware.py` : méthode exécutée `python3 comparison.py initial`, chemins/guardUID/commandes archivés. Le mode recovery est retiré ; aucune fonction flash appelée.
- `manifest.json` : SHA-256 de tous les fichiers du dossier sauf lui-même. Logs Windows/GDB conservés byte-identiques CRLF ; pas de remplacement d'erreurs.

Redémarrage électrique déclaré par l'utilisateur dans le contrat, non mesuré par Codex. Firmware, SD, broches, fréquence et USB inchangés. Debug ponctuel halt/detach ; son influence n'est pas exclue, mais No Media préexiste à cette session. Carte seulement identifiée par handle initialisé ; cet état ne prouve pas son accessibilité. BIN/readback tiers restent locaux, non redistribués. Ne pas répéter cette séquence après le critère d'arrêt sans nouvelle mission.
