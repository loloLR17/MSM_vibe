# FT-STR-05 — Registres réservés et sentinelles structurelles

## 1. Identification

- **ID** : FT-STR-05
- **Famille** : FT-STR
- **Criticité** : P0

## 2. Objectif

Vérifier que les zones explicitement réservées par V1 sont identifiées sans ambiguïté, conservent leur géométrie normative et respectent la valeur structurelle imposée. Vérifier également qu'aucune sentinelle globale n'est inventée pour les champs non réservés.

## 3. Règle de vérité

V1 est normative. GEL-MAP-V1 est dérivé. En cas de contradiction, V1 prévaut et l'écart doit être signalé ; il ne doit jamais être corrigé silencieusement dans la validation.

## 4. Inclus

- identification des champs/plages explicitement réservés ;
- adresses, tailles et types documentaires de ces plages ;
- valeur `0` lorsqu'elle est imposée par V1 ;
- absence de règle globale `0 = absent / invalide / non renseigné` ;
- sentinelles explicitement définies champ par champ par V1.

## 5. Exclus

- stabilité temporelle et répétition des lectures : FT-STR-07 ;
- accessibilité et découpage des lectures : FT-STR-06 ;
- refus d'écriture sur réservés : FT-ACC / GEL-GOV-02 ;
- logique métier des valeurs hors sentinelles explicitement normées.

## 6. Doctrine

Un nom, une description ou une habitude historique ne suffit pas à créer une sentinelle. La valeur `0` garde sa valeur numérique normale sauf règle V1 explicite.

Les zones réservées sont des emplacements structurels sans sémantique métier. Lorsque V1 impose leur lecture à zéro, toute valeur non nulle sur la cible est non conforme.

## 7. Cas génériques

- `TT-STR-05-GEN-001` — Identification exhaustive des zones réservées.
- `TT-STR-05-GEN-002` — Valeur structurelle nulle des zones réservées.
- `TT-STR-05-GEN-003` — Absence de sentinelle implicite.

## 8. Instanciation

L'instanciation active s'appuie directement sur les lignes GEL-MAP-V1 dont le champ est explicitement réservé. Elle ne duplique pas une fiche Markdown par plage.

Le validateur mécanique vérifie la sélection, les noms, les plages, les tailles, les types `uint16` / `uint16[n]`, l'accès RO documentaire et la couverture attendue. Il ne peut pas démontrer la valeur réellement retournée par un firmware.

## 9. Réussite

- toutes les zones réservées V1 sont représentées une fois dans GEL-MAP-V1 ;
- aucune zone historique obsolète ou dupliquée n'est active ;
- la géométrie et le type documentaire sont cohérents ;
- les contrôles terrain confirment `0` pour chaque registre réservé lorsque V1 l'impose ;
- aucune sentinelle implicite n'est introduite.

## 10. Ambiguïté

Toute signification de sentinelle absente de V1 est classée `NON DÉFINI / À ARBITRER`.
