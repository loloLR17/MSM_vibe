# R0 — preuves documentaires

Référence inspectée : `2b9c0202f79bc132b166776c4aed2a48925593dc`. Mission sans flash/debug, aucun accès matériel.

- `initial-state.json` : état initial, stashes et 2 940 empreintes des fichiers historiques protégés. Les nouveaux documents LIVE sont supplémentaires, pas des modifications de ces fichiers.
- `corpus-reuse.json` : comparaison de chacun des 2 167 contenus inventoriés A06 ; corpus inchangé, analyse R0 ciblée sur les nouveaux choix normatifs et leur cohérence.
- `namespace-review.json` : sources des allocations, distinction produit/fixtures et V1/V1.1/LIVE.
- `check_documents.py` : vérification reproductible sans imports firmware ni matériel. Depuis la racine du worktree, exécuter `python3 'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261015-R0/check_documents.py'`.
- `document-checks.json` : résultat effectif des vérifications. Les 106 scénarios fonctionnels et 384 contextes sont des oracles, pas des tests exécutés.
- `layout-review.json` : calcul des offsets depuis constantes inspectées, absence de recouvrement et contrôle de tous les préfixes des écritures prévues. Ce modèle ne teste pas l'atomicité de la FRAM ou les stores C.
- `MANIFEST.sha256` : empreintes des preuves, hors manifest lui-même ; les reçus de relecture distante sont ajoutés lors de la clôture.

Résultats numériques : mapping187champs/390registres ; CRC V1 préparé5207CCFC, LIVE_A7BA855C8/7818AAB3, LIVE_BFC4001B4/E5F05014 (préparé/actif). Les tableaux complets sont dans `01_Specification_source/LIVE_R0/vectors.json`. Toute qualification logicielle/physique reste future.
