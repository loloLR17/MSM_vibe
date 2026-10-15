# Preuves documentaires A-06

Référence examinée : main `45ee898a6ebe9269e63904262dc9f1b9c8b87c78`.
Aucun essai physique, test firmware/PC ni compilation dans cette mission.

- `corpus-inventory.csv` : lecture intégrale des 2 167 fichiers sous les deux racines imposées, empreintes et catégories structurelles ; aucun fichier supprimé.
- `corpus-summary.json` : comptes et limites de méthode.
- `dependency-occurrences.csv` : index de toutes les lignes correspondant aux termes de dépendance, pas une extraction normative faisant autorité.
- `active-headings.csv` / `requirements-index.csv` : index de revue documentaire, non preuves de couverture exécutée.
- `file-impact-screening.csv` : criblage conservateur par famille, tous les chemins exacts ; « à modifier » et « gel impacté » concernent un successeur proposé, jamais une modification effectuée en V1. Les variantes de tests sans média sont à ajouter seulement après décision normative.
- `traceability.csv` : 20 relations vérifiées exigence → tests → code → impact → proposition ; la note développe les oracles et sépare tests existants/nouveaux.
- `preservation-baseline.json` : 2 939 empreintes de fichiers protégés, HEAD historique, stashes et worktrees conservés.
- `document-checks.json` : résultats effectifs des vérifications documentaires ; aucun PASS de fonctionnalité.
- `MANIFEST.sha256` : intégrité des preuves, hors manifest lui-même ; un reçu distant ultérieur peut compléter ce manifest.

Reproduire le parcours sans accès matériel depuis un checkout de la référence examinée : copier `inventory-script.py` vers `/tmp/a06_scan.py`, puis lancer `python3 /tmp/a06_scan.py` depuis la racine du checkout. Le script écrit uniquement dans le répertoire de preuves A-06. Il classe toutes les variantes historiques ; un doublon de contenu n'autorise ni fusion ni suppression. La relecture sémantique et les recommandations ne sont pas automatisées par ce script.

Les CSV livrés sont sérialisés avec champs entre guillemets et fins de ligne LF. Le premier contrôle de diff avait signalé les CRLF produits par le writer CSV standard ; seule la sérialisation des nouvelles preuves a été normalisée, aucun contenu source ou archive modifié.
