# FT-ACC-04 — Fiche de spécification

## Comportement des registres réservés

## 1. Identification
- **ID** : FT-ACC-04
- **Nom** : Comportement des registres réservés
- **Famille parente** : FT-ACC
- **Criticité** : P0

## 2. Objectif
Valider que toute zone déclarée réservée :
- reste neutre ;
- conserve un comportement déterministe ;
- n’expose aucune sémantique cachée ;
- ne peut pas être modifiée de manière illégitime.

## 3. Périmètre

### Inclus
- lecture des réservés si exposés ;
- réservés unitaires et multi-registres ;
- tentative d’écriture sur réservés ;
- stabilité read → read ;
- stabilité write-attempt → read ;
- cohérence gouvernance ↔ comportement observé.

### Exclus
- hors plage ;
- robustesse communication ;
- effets de bord détaillés ;
- validité métier de champs non réservés.

## 4. Références d’entrée
- Mapping unifié logique TR2
- FT-STR validée
- FT-ACC-01 validée
- Gouvernance : tout registre réservé est nommé `reserved*`

## 5. Règles de conformité
Un registre réservé est conforme si :
- son comportement reste cohérent et déterministe ;
- aucune tentative d’écriture n’aboutit à une modification illégitime ;
- aucune sémantique cachée n’est observée ;
- la gouvernance documentaire est respectée.

## 6. Préconditions
- mapping stabilisé ;
- accès Modbus opérationnel ;
- simulateur en état stable ;
- valeur initiale lisible.

## 7. Résultats attendus
- réservés identifiés sans ambiguïté ;
- lecture stable ;
- tentative d’écriture refusée ou sans effet observable ;
- aucune divergence documentaire non justifiée.

## 8. Axes de couverture
- lecture réservés ;
- réservés en plage ;
- tentatives d’écriture ;
- répétabilité ;
- stabilité ;
- conformité mapping ↔ comportement.

## 9. Critères d’acceptation
La sous-famille est satisfaite si :
- tous les `reserved*` du mapping ont un comportement déterministe ;
- aucune tentative d’écriture n’altère leur valeur ;
- aucune sémantique cachée n’est révélée par l’accès.

## 10. Automatisation
Oui, fortement automatisable.

## 11. Conclusion
FT-ACC-04 verrouille la neutralité et la stabilité des réservés, base importante pour la robustesse documentaire et l’évolutivité du protocole.
