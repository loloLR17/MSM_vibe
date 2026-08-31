# FT-LIM-05 — Source normative

## Références

- V1 Bloc 6 — Inventaire campagnes.
- Mapping Modbus unifié V1.
- GEL-GOV-02 pour la distinction accès Modbus invalide / valeur fonctionnelle invalide.

## Exigences

### LIM05-RQ-001 — Domaine dynamique
Pour `N = total_campaign_count`, `selected_campaign_index` est fonctionnellement valide exactement dans `0..N-1`.

### LIM05-RQ-002 — Index hors plage
Une écriture hors plage sur 6003 reste une écriture Modbus valide. Elle est acceptée sans exception Modbus due à la valeur et entraîne `selected_campaign_valid=0`.

### LIM05-RQ-003 — N nul
Si `N=0`, aucun index n’est fonctionnellement valide, y compris 0.

### LIM05-RQ-004 — Sélection valide
Pour un index valide, `selected_campaign_valid=1` et les métadonnées exposées correspondent à la campagne sélectionnée.

### LIM05-RQ-005 — Identifiant campagne
Pour une campagne sélectionnée valide, `campaign_id` est non nul.

### LIM05-RQ-006 — Sélection invalide
Lorsque `selected_campaign_valid=0`, les métadonnées de l’entrée sélectionnée ne doivent pas être interprétées comme valides. La V1 ne fixe pas leur valeur numérique exacte : FT-LIM-05 n’en invente aucune.

### LIM05-RQ-007 — Cohérence de sélection
Les champs exposés pour une sélection valide doivent appartenir à une seule campagne. Un changement de sélection ne doit pas créer un mélange de campagnes dans une même réponse Modbus.

## Limites de représentation

`selected_campaign_index` et `total_campaign_count` sont des uint16. Les cas de frontière sont construits uniquement lorsqu’ils sont représentables. Aucun cas N=65536 n’est inventé.

## Non-redondance

FT-LIM-05 ne reteste pas les permissions d’écriture du Bloc 6 ni l’atomicité structurelle générale des lectures. Ces propriétés sont respectivement couvertes par FT-ACC et FT-STR.
