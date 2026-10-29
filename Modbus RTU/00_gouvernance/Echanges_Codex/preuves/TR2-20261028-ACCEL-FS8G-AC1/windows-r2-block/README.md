# Déploiement R2 refusé — 29 octobre 2026

Mission TR2-20261028-ACCEL-FS8G-VIBRATION-AC. Paquet Windows complet issu de ae6e2d5e, 378 fichiers copiés puis hashés. Gate COM99 sans accès au TR2 : échec HTTP503, SAC1, 296 événements corrélés3033/3077 (292 NModbus.Serial.dll, 4 SQLitePCLRaw.batteries_v2.dll). Nombre d’événements, pas nombre de composants distincts. Les deux DLL sont identiques en SHA-256 à R1 ; leur présence dans un autre paquet précédemment accepté ne garantit pas cette exécution. Aucune cause unique de réputation/signature déduite.

Les fichiers `.zlib` sont des compressions zlib sans transformation. BUNDLE donne empreintes brutes et stockées. Les XML Windows intégraux restent dans les preuves locales : la publication utilise un extrait structuré conservant ID/record/UTC/fichier/statut/politique/hash, sans SID ni nom du PC. Les sauvegardes Flash complètes/FRAM/SQLite restent locales et sont référencées par empreintes, sans substitution d’une empreinte à une inspection.

`final-b3-api.json` prouve seulement l’observation réelle STOP/B3 via l’ancien service accepté, pas AC1 ni ±8g. `history-preservation-resume.json` consigne la comparaison SQL intégrale des 52824 lignes originales dans la sauvegarde R1 de75006 lignes. Aucun nouveau test firmware, START/STOP, APPLY, flash ou reset à la reprise. Les preuves du premier démarrage perturbé sont dans `../resume`.
