# FT-CMD-06 — Acquisition et diagnostic

## Objet

Cette sous-famille valide les règles transactionnelles normatives des commandes :
- `3` START ACQUISITION ;
- `4` STOP ACQUISITION ;
- `5` SELFTEST ;
- `6` ACK défaut / alarme ;
- `7` REFRESH indicateurs.

FT-CMD-06 couvre les préconditions, acceptations/refus et codes résultat définis dans le Bloc 5. Les effets observables dans les Blocs 1, 6 et 7 restent à FT-INT.

## Classification stricte

### START ACQUISITION
- configuration active valide requise : `COVERED` ;
- SD exploitable requise : `COVERED` ;
- mémoire suffisante : `COVERED` ;
- absence de défaut critique bloquant : `COVERED` ;
- acquisition déjà active / contexte incompatible : `COVERED` via code `3` si le cas est construit sans autre cause prioritaire ;
- code `22` en absence de configuration active valide : `COVERED`.

### STOP ACQUISITION
- acquisition active requise : `COVERED` ;
- code `21` si acquisition non active : `COVERED`.

### SELFTEST
- commande et résultat transactionnel Bloc 5 : `COVERED` ;
- publication du résultat détaillé en Bloc 7 : `DELEGATED` à FT-INT ;
- masque `param1 != 0` : `CONDITIONAL`, car l'extension n'est applicable que si implémentée.

### ACK défaut / alarme
- défaut présent et acquittable : `COVERED` ;
- code `16` si non acquittable : `COVERED` ;
- code `2` si paramètre invalide : `COVERED` lorsque le cas est constructible ;
- suppression de la cause : `DELEGATED` à FT-INT, avec règle normative de non-suppression si la cause persiste.

### REFRESH indicateurs
- existence et succès transactionnel : `COVERED` ;
- absence de modification de configuration : `DELEGATED` à FT-INT ;
- détails exacts des indicateurs recalculés : `NOT_DEFINED` dans le Bloc 5.

## Tests instanciés

- `TT-CMD-B05-500` à `TT-CMD-B05-506` : START ACQUISITION ;
- `TT-CMD-B05-507` à `TT-CMD-B05-508` : STOP ACQUISITION ;
- `TT-CMD-B05-509` à `TT-CMD-B05-511` : SELFTEST ;
- `TT-CMD-B05-512` à `TT-CMD-B05-514` : ACK ;
- `TT-CMD-B05-515` : REFRESH.

## Frontière FT-INT

Le passage de `acquisition_state`, l'ouverture/fermeture de campagne, la publication de l'autotest en Bloc 7, l'effet de l'ACK sur les défauts et les non-effets inter-blocs de REFRESH ne sont pas ré-audités ici.

## Statut

Reconstruite sur branche d'audit. En attente de validation avant merge.
