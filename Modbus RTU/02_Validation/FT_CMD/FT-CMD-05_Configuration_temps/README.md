# FT-CMD-05 — Configuration et temps

## Objet

Valider les règles transactionnelles des commandes Bloc 5 :
- code 1 — appliquer la configuration préparée ;
- code 2 — synchroniser l'heure.

FT-CMD-05 couvre les préconditions, acceptations et refus. Les effets réellement observés dans les Blocs 4 et 2 restent à FT-INT.

## Références normatives

- `01_Specification_source/bloc5.md`
- `01_Specification_source/bloc4.md`
- `01_Specification_source/bloc2.md`

## Exigences principales

### APPLY CONFIG
- acquisition arrêtée ;
- configuration préparée complète ;
- contenu Bloc 4 valide ;
- CRC préparé recalculé au moment de l'application et cohérent ;
- une configuration ne peut être appliquée si `config_state != VALIDE` ;
- refus normatifs associés : `3`, `4`, `5`, `20` selon la cause.

### SYNC TIME
- heure préparée présente ;
- horloge disponible ;
- refus normatifs associés : `19` et `12`.

## Frontières

FT-CMD-05 ne revalide pas :
- les domaines simples du Bloc 4 : FT-LIM ;
- la structure/encodage : FT-STR ;
- les accès RW/RO : FT-ACC ;
- les transitions inter-blocs réussies et effets sur `current_time`, `last_sync_time`, configuration active : FT-INT.

## Tests instanciés

- `TT-CMD-B05-400` — APPLY CONFIG nominalement admissible ;
- `TT-CMD-B05-401` — APPLY refusée acquisition en cours, code 5 ;
- `TT-CMD-B05-402` — APPLY refusée configuration préparée incomplète, code 20 ;
- `TT-CMD-B05-403` — APPLY refusée configuration invalide / CRC incohérent, code 4 ;
- `TT-CMD-B05-404` — APPLY refusée état incompatible, code 3 lorsque ce cas est distinctement constructible ;
- `TT-CMD-B05-405` — SYNC TIME nominalement admissible ;
- `TT-CMD-B05-406` — SYNC refusée sans temps préparé, code 19 ;
- `TT-CMD-B05-407` — SYNC refusée horloge indisponible, code 12.

## Statut

Reconstruite sur branche d'audit. En attente de validation avant merge.
