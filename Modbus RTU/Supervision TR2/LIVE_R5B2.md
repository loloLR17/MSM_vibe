# R5-B2 — chaîne physique LIVE vers le service et le Web

Mission `TR2-20261018-R5B2-PHYSICAL-LIVE-RESUME`, HEAD initialdc47ee0. Rapport détaillé et critères : [DERNIER_RAPPORT](../00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md). Les preuves brutes, l'oracle et les corrélations sont dans [R5B2](../00_gouvernance/Echanges_Codex/preuves/TR2-20261018-R5B2/README.md).

Le vrai service Windows local a reçu les mesures physiques sur COM4 en1152008E1, via le transport NModbus et le lecteur strict existants. Firmware et service dc773733 inchangés ; contrôleur de campagne7bf84b1. Les372 fichiers de la copie locale sont identiques à la publication déjà évaluée. Aucun rebuild,561 tests .NET ou sept scénarios JS répétés. Le lecteur accepte les vrais B0/B4/B6/B3 avec opt-in LiveDiagnostic ; aucun header simulé ou décodeur affaibli.

Une seule paire START5/12/run11 et STOP5/13 a été exécutée, terminaux4/0. Un seul maître par phase, ouverture concurrente refusée sans transmission, fermeture vérifiée avant chaque passage.824 ADU du service/206 B3 et1033 ADU du maître explicite, CRC exacts. SQLite conserve234 observations COM4 :64 initiales,88 courantes,28 pertes logiques sans snapshot et54 HELD.141 lignes API sont corrélées à SQLite ; Edge headless a rendu un vrai résultat V/F. Les294 observations COM99 des deux smokes sont identifiées séparément et ne contiennent aucune mesure.

Les RMS globaux courants réellement observés varient de1046 à1092mg (neuf valeurs). Pas de vibration imposée à l'utilisateur. UTC fenêtre indisponible, identités run/epoch/config non exposées restant null. Une déconnexion logique ferme le handle pendant3s ; la reconnexion ouvre un second segment PC, sans prétendre un reboot MCU. Le moteur Edge headless a exécuté le JS et rendu le DOM ; aucun navigateur humain ni screenshot revendiqué.

Budgets mesurés : work810µs/service2029µs/gapFIFO2330µs, limites1000/2500/5000 conservées ;2301494 XYZ/287687 timestamps/FIFOmax70, erreurs0. Oracle4096 indépendant exact : RMS1088/82/1082/79mg, peaks1173/151/1165/160mg. Les terminaux START et STOP prennent chacun environ39,9s, pas une qualification de sécurité industrielle.

M016 restauré17:09:24UTC le18/10 : Flash2Mio exacte c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26,19 pages normales. FRAM finale entière2b501a835b9a02c43dc0738bd7af167b7d049fbffb4e9ef677ed2e867f47dd84 exacte au modèle et relue après restauration sans rollback. Historique/tail immuables, journal78/root91/epoch5/run11/config3gen3/STOP5/13. Capteur arrêté/WHO7B, GDB détaché, COM libéré. Aucun SD/HP/câblage/mass erase/Option Bytes/protection modifiée.

## Exécution reproduite et garde de reprise

Service utilisé : `C:\Users\lolo\AppData\Local\MSM\TR2-r5b\TR2.Supervision.Service.exe`. Base, traces neuves et contrôle de connexion sous le répertoire frère `TR2-r5b2-evidence`. Paramètres réellement utilisés :

```text
--live-direct COM4 --allow-diagnostic --database <live.db> --listen http://127.0.0.1:5089 --trace <trace-neuve.jsonl> --seconds <12|20|10> --unit 1 --connection-control <fichier>
```

Séquence exécutée : service initial12s ; fermeture ; maître explicite START ; fermeture ; service RUNNING20s avec fichier `disconnected` entre5..8s ; fermeture ; maître explicite STOP ; fermeture ; service HELD10s dans la fenêtre20s du probe ; checkpoint final ; contrôles FRAM/oracle ; restauration. Les commandes et octets exacts sont conservés dans `common.py`, `hardware/run.py`, les JSON de lancement et les traces. Aucune commande B5/FC10 du service ou de l'UI.

**Ne pas relancer ces scripts inchangés** : les clés5/12–5/13 sont consommées et la FRAM initiale4e7fab du probe est historique. Toute campagne suivante exige une garde fraîche depuis2b501a et des identités admissibles. Le probe de test n'est plus sur la carte, M016 n'expose pas ce profil LIVE. Après une nouvelle autorisation et préparation adaptée seulement, utiliser le chemin Windows accepté, une trace neuve et un seul maître.

Limites : firmware diagnostique partiel, B0 sans LIVE16, B1/B2/B7 absents, B6 UNAVAILABLE préparé, UTC absente, N4096 et essai court. Aucun produit permanent, endurance, calibration, CEM, robustesse électrique, WCET en faute ou qualification R6/industrielle. Prochaine étape : contrôle indépendant ChatGPT et choix du contrat suivant. Le checkpoint R5-B historiquement BLOCKED reste archivé sans réécriture.
