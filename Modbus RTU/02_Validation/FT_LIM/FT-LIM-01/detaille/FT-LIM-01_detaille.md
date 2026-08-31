# FT-LIM-01 — Cas génériques détaillés

## 1. Préconditions communes

Sauf mention contraire :

1. disposer d'une configuration active valide A ;
2. préparer une copie B complète et connue valide ;
3. acquisition arrêtée ;
4. utiliser un nouveau `prepared_config_id` non nul ;
5. ne modifier dans B que la cible du test ;
6. recalculer `prepared_config_crc` après toute modification de 4B+4C+4D ;
7. utiliser un nouveau `transaction_id` pour la commande Bloc 5 ;
8. conserver un snapshot de l'image active A avant l'essai.

La commande utilisée comme oracle fonctionnel est la commande Bloc 5 code `1` « appliquer configuration préparée ».

Le test FT-LIM-01 ne revalide pas la permission RW : il vérifie le traitement fonctionnel de la valeur.

## 2. Verdict commun — valeur invalide

Pour toute instance déclarée invalide :

1. écrire la valeur cible dans le champ RW ;
2. vérifier que l'écriture Modbus n'est pas rejetée du seul fait du domaine métier ;
3. vérifier le passage de la configuration préparée en `BROUILLON` après modification ;
4. mettre à jour le CRC préparé si la cible appartient à 4B+4C+4D ;
5. demander l'application ;
6. vérifier que la configuration n'est pas appliquée ;
7. vérifier que l'image active reste identique au snapshot A ;
8. vérifier qu'aucune exécution partielle n'est observable.

Lorsque la V1 ne fixe pas un code résultat unique pour la cause précise, FT-LIM-01 n'en invente pas. Le critère primaire est l'absence d'application de la configuration invalide et la conservation de l'image active.

## 3. Verdict commun — valeur valide

Pour toute instance déclarée valide :

1. partir de la baseline B valide ;
2. substituer uniquement la valeur candidate ;
3. mettre à jour le CRC préparé si nécessaire ;
4. demander l'application ;
5. vérifier que la valeur candidate ne provoque pas à elle seule un échec de validation ;
6. si toutes les autres préconditions normatives sont satisfaites, l'application doit réussir et l'image active doit refléter la valeur candidate.

Une instance `VALID_IF_*` est exécutée uniquement lorsque la condition indiquée est satisfaite.

## 4. Cas génériques

### LIM01-G01 — Domaine singleton

Usage : `sampling_frequency_hz`.

Preuve recherchée :
- la valeur normative unique est acceptée fonctionnellement ;
- les valeurs immédiatement adjacentes ne sont pas validables.

### LIM01-G02 — Ensemble discret / enum

Usage :
- `full_scale_code` ;
- `acquisition_mode` ;
- `window_size_samples` ;
- `indicator_period_ms` ;
- `storage_mode`.

Preuve recherchée :
- les membres explicitement autorisés sont acceptés ;
- les sentinelles « non configuré » restent non validables lorsqu'elles sont définies ainsi ;
- des représentants des valeurs réservées / non membres sont refusés fonctionnellement.

L'exhaustivité logique du domaine est portée par la règle source ; les valeurs de test sont choisies pour démontrer les classes pertinentes sans multiplier artificiellement les fiches Markdown.

### LIM01-G03 — Bitfield avec bits réservés

Usage : `axes_enable_mask`.

Preuve recherchée :
- les 7 combinaisons non nulles des bits X/Y/Z sont valides ;
- `0x0000` est invalide ;
- chacun des bits réservés 3..15, testé isolément, rend la valeur invalide.

### LIM01-G04 — Intervalle borné inclusif

Usage : `campaign_duration_s`.

Valeurs de preuve :
- juste sous minimum ;
- minimum ;
- nominale ;
- maximum ;
- juste au-dessus du maximum.

### LIM01-G05 — Sentinelle zéro / identifiant requis

Usage :
- `prepared_config_id` ;
- `campaign_context_id` ;
- `mission_id`.

Preuve recherchée :
- `0` est accepté au transport mais signifie « non renseigné » et ne permet pas une configuration validable ;
- `1` prouve la borne minimale non nulle sans prétendre limiter la borne haute du type `uint32`.

### LIM01-G06 — Domaine statique partiel avec contrainte dynamique

Usage : `storage_limit_mb`.

Preuve recherchée dans FT-LIM-01 :
- `0` est invalide ;
- `1` est une valeur candidate valide si la capacité utilisable du firmware est au moins 1 MB.

La preuve de compatibilité avec la capacité réelle pour des valeurs supérieures est différée à FT-LIM-02 ; aucune capacité arbitraire n'est codée ici.

## 5. Critères d'acceptation

Une instance est `PASS` uniquement si tous ses critères primaires sont satisfaits.

Une valeur invalide donnant une exception Modbus uniquement parce qu'elle est hors domaine métier est `FAIL`.

Une valeur invalide devenant active, même partiellement, est `FAIL`.

Une valeur classée `NOT_DEFINED`, `DEFERRED` ou `STRUCTURAL` dans la matrice ne doit pas apparaître comme instance active FT-LIM-01.
