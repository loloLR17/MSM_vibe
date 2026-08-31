# FT-ACC-06 — Fiche de spécification

## Accès hors plage, partiels ou non autorisés

## 1. Identification
- **ID** : FT-ACC-06
- **Nom** : Accès hors plage, partiels ou non autorisés
- **Famille parente** : FT-ACC
- **Criticité** : P0

## 2. Objectif
Valider le comportement du système lorsqu’une requête Modbus ne respecte pas exactement le contrat d’exposition du mapping.

## 3. Doctrine de référence
Toute requête invalide doit :
- renvoyer une exception Modbus explicite ;
- ne produire aucune modification mémoire ;
- rester déterministe d’une tentative à l’autre.

Aucune exécution partielle silencieuse n’est autorisée.

## 4. Périmètre
### Inclus
- hors plage total ;
- hors plage partiel ;
- débordement de début ;
- débordement de fin ;
- franchissement de frontière logique ;
- franchissement de bloc ;
- longueur excessive ;
- contrôle mémoire avant/après.

### Exclus
- trame Modbus malformée ;
- défauts CRC / RS-485 ;
- robustesse communication ;
- invalidité métier sur cible RW valide.

## 5. Critères d’acceptation
- exception Modbus explicite sur requête invalide ;
- aucune modification mémoire ;
- comportement cohérent avec le mapping ;
- comportement déterministe.

## 6. Automatisation
Oui, fortement automatisable.

## 7. Conclusion
FT-ACC-06 verrouille la discipline d’adressage et l’absence d’exécution partielle implicite.
