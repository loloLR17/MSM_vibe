# FT-LIM-03 — Vue d'ensemble des instances

## Couverture active

FT-LIM-03 contient 17 instances couvrant 12 cas génériques.

| Domaine | Instances | Objet |
|---|---:|---|
| CRC normatif et détection d'incohérence | 4 | vecteur de référence, CRC faux, CRC corrigé |
| Périmètre CRC | 2 | inclus 4B+4C+4D, exclu 4A |
| Machine d'état validation/application | 8 | BROUILLON, validation, erreurs, récupération, interdiction VIDE→ACTIF |
| Cohérence/protection image active | 3 | préparation depuis ACTIF, atomicité, active_config_crc |
| **Total** | **17** | |

## Cas conditionnels

Deux instances sont conditionnelles :
- TT-LIM-03-013 : provoquer un échec réel après validation réussie ;
- TT-LIM-03-014 : vérifier le retour ERREUR_APPLICATION → BROUILLON.

Elles deviennent N/A si l'environnement ne permet pas d'injecter proprement une défaillance d'application distincte d'une erreur de validation.

## Points critiques couverts

- vecteur CRC V1 = `0x5207CCFC` ;
- recalcul indépendant par le firmware lors de l'application ;
- absence d'activation avec CRC faux ;
- absence de transition directe `VIDE -> ACTIF` ;
- retour à `BROUILLON` après modification d'une préparation précédemment validée ;
- préservation intégrale de l'image active en cas d'échec ;
- mise à jour cohérente et non partielle de 4E en cas de succès ;
- cohérence de `active_config_crc` avec 4E.

## Non-invention

- aucune table de codes n'est créée pour `config_error_code` ;
- aucune durée minimale de visibilité de l'état `VALIDE` n'est imposée ;
- aucune cause artificielle d'`ERREUR_APPLICATION` n'est inventée ;
- l'incomplétude n'est testée que lorsqu'elle est démontrable à partir d'une exigence normative existante.

## Frontière avec FT-LIM-04

FT-LIM-03 utilise uniquement la commande 1 comme mécanisme d'application. Les règles génériques de moteur de commandes, transaction, submit, annulation, confirmation et autres codes de commandes restent à couvrir dans FT-LIM-04.
