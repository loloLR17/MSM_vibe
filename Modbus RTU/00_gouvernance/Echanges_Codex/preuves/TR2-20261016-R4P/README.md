# R4-P — preuves

Le rapport courant distingue les étapes acquises, les erreurs et les opérations
non exécutées. Logs GDB bruts, commandes/invocations, source/build/empreintes,
snapshots FRAM sans perte et comparaisons complètes servent au contrôle indépendant.
Les identifiants r4m_* des variables réutilisées ne désignent pas un ancien essai :
les scripts r4p-* et le code/build-artifacts.json identifient cette campagne R4-P.
expected-final-MODEL-ONLY est un oracle RAM, jamais un relevé STM32.
La trace initiale authentique STARTED2/run2 reste intégralement archivée dans
../../../../05_Firmware/tests/fixtures/r4p_started_snapshot.zlib (SHA f2bd7a99…8a0c1).
Les .zlib sont sans perte (zlib.decompress),262144octets pour chaque snapshot FRAM.
Les doubles backups Flash2Mio et FRAM et la lecture Flash restaurée sont récupérables
sous /home/lolo/dev/msm/reports/TR2-r4p/hardware/ ; hashes/contrôles publiés.
L'image du probe NON-PRODUCTION est fournie pour vérification, pas pour exploitation.
SHA256SUMS.json vérifie tous les fichiers complets sauf lui-même. CRLF Windows bruts
conservés. Publication/relecture ne valent pas qualification indépendante R4/R6.
