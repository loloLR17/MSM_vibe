# Preuves R3-A

Code évalué0c6f002fd9cc09f577ff9ec446b5af46d5393c27 ; basec5c1c22.
Logs full/final/sanitizer/targeted, profil et hashes ARM ; aucune exécution cible.
`r3a-loc-final.json` : métrique reproductible des trois références Git avec script
commité. `git-numstat.txt` : ajouts/suppressions distincts des LOC.
`r3a-module-inventory.json`, `link/production-linked-symbols.json` : inventaire
et preuve des unités possédant des symboles dans l'ELF canonique ; pas de code
mort déduit d'une absence. `shared-bodies.json` : corps des fonctions réutilisées
identiques au départ. `link/link-check.log` : commande de lien forcé et SHA,
**NON DESTINÉ AU FLASH**, pas une composition opérationnelle. `resources` : tailles
ABI/pile statique, pas un high-water dynamique.
`initial-state.json` : références/worktrees/stashes et empreintes protégées.
`evaluated-source-sha256.json` : fichiers firmware et source ST propriétaire évalués.
`r3a-hardware-inventory.json` : seule énumération STLINK, sans connexion cible.
`verification.json` : synthèse/limites. Les logs intermédiaires ne sont pas tous
le dernier passage ; host-final et arm-final sont les résultats finaux, sanitizer
couvre owner/port/intégration hôte. Aucune qualification physique présumée.

`remote-readback.json` consigne la relecture intégrale depuis GitHub du commit a33066d, avant la mise à jour DONE. La dernière publication est vérifiée séparément et son SHA fourni dans la réponse finale.
