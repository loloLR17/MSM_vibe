# TR2 — Mapping unifié

## Contenu
- `tr2_mapping_unifie_brut.csv` : une ligne par champ source après déduplication des répétitions documentaires.
- `tr2_mapping_unifie_logique.csv` : vue logique avec regroupements sûrs (`*_msw`/`*_lsw`, `*_rN`).
- `tr2_mapping_couverture.csv` : couverture par bloc.
- `VERSION.md` : statut.

## Convention de nommage
- Convention de gouvernance : tout champ logique réservé doit être nommé `reserved_*` dans `tr2_mapping_unifie_logique.csv`.

## Note
Les documents source contiennent parfois à la fois un tableau de synthèse et un tableau complet ; la présente version retire ces doublons exacts.
