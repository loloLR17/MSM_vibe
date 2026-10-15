# Preuves R3-B

Résultats logiciels uniquement. `verification.json` distingue code testé et retrait final d'une ligne blanche, tailles et limites. `final-host-tests.log` :133/133 ; `final-target-outputs.log` : stdout exact des trois tests nouveaux/étendus extrait du LastTest.log final, sans les rejouer. `final-arm.log`, sanitizer et coûts R2 conservés.

`shared-functions.json` prouve les corps inchangés ; `projection-readiness.json` suit187champs R0 sans modifier leur mapping. `loc-final.json` compare avant R1/R2/R3-A/résultat ; HEAD y désigne le SHA de verification.json, avant commits de rapport. Numstat distinct, JSON n'est pas du code de production. Inventaire414unités/interfaces/tests, pas de code déclaré mort.

`resources` : commandes ARM-O0, tailles/.su et lien forcé distinct NON DESTINÉ AU FLASH. Aucun nouvel objet instancié dans le canonique ; tailles statiques ne prouvent pas les marges CPU/pile/IRQ. Scripts de mesure copiés pour traçabilité, leurs chemins de sortie externes sont ceux réellement utilisés.

Les logs d'échec sont préservés : inclusion imbriquée du main de fixture avant extraction d'un wrapper de test, puis test synthétique tentant un retour de temps à0 après redémarrage ; corrigé en conservant l'oracle de monotonie stricte. Aucune erreur masquée ni test supprimé. Les normes ont été lues avant développement, mais les vecteurs Python ont été matérialisés après la première rédaction du service, avant sa compilation : écart méthodologique explicité dans LIVE_R3B.md.

Manifeste SHA256SUMS.json pour chaque fichier sauf lui-même. Aucun flash/debug/reset, accès FRAM/SD ou émission RS485 dans cette mission.
