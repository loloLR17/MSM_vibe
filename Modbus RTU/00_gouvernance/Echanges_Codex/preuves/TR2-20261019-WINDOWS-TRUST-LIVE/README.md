# Preuves WINDOWS-TRUST-LIVE-RESUME

Mission TR2-20261019-WINDOWS-TRUST-LIVE-RESUME. L’utilisateur autorise la qualification physique limitée au paquet c36c971 accepté sous protection Windows inchangée ; correction8a54/rebuild refusé conservés séparément, signature durable reportée.

`BUNDLE.json` donne les empreintes brutes/stockées. Les `.zlib` se décompressent par `zlib.decompress` ; aucun octet CRLF/binaire n’est normalisé. Les logs de refus/échecs sont conservés. Le rapport courant distingue checkpoint et résultat final.

- `checkpoint-gate/` : smoke réel Windows COM99, quatre HTTP200, SAC1, zéro refus corrélé,377 fichiers inchangés.
- `rebuild-gate/` : nouveau Service.dll refusé par CodeIntegrity, non utilisé physiquement. `trust-comparison`, `policy-*`, `project-signatures` : inspection non destructive, aucun certificat de signature accessible/droit administrateur.
- `operator-decision.json` : démo limitée explicitement autorisée ; aucun contournement ni assurance future.
- `hardware/` : préflight neuf, sauvegardes vérifiées, flash LIVE9bbe…0981/readback exact, bootSTOP ; audits uniquement arrêtés, FRAM/XYZ et coûts réels. Le reset logiciel documenté vérifie le reboot arrêté, jamais un rollbackFRAM.
- `physical/` : commandes natives c36c971 avec traces série, terminal+effet et handoff exclusif ; service réel COM4/SQLite/HTTP5100 ; Edge headless, DOM/API/CSV/PNG. Les observations de pause restent des pertes logiques sans mesures.
- `service-tests/` :239 tests ciblés pour correction acquittement stable ; autres validations inchangées réutilisées.

Les scripts externes sont des recettes et preuves, pas une autorisation de rejouer les clés déjà consommées. Lire la mission active, l’état réel et le ledger avant toute opération. La SQLite finale est copiée après fermeture du service ; aucune archive ne remplace le flux COM4 réel.
