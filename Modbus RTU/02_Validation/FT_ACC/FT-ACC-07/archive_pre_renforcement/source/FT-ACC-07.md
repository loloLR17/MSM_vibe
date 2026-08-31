# FT-ACC-07 — Fiche de spécification

## Conformité mapping ↔ permissions observées

## 1. Identification
- **ID** : FT-ACC-07
- **Nom** : Conformité mapping ↔ permissions observées
- **Famille parente** : FT-ACC
- **Criticité** : P0

## 2. Objectif
Valider que le comportement réel du système en lecture et en écriture est strictement conforme au mapping unifié.

## 3. Règles de référence
- **RO** : lecture OK, écriture refusée avec exception explicite, aucune modification mémoire.
- **RW** : lecture OK, écriture OK, write → read cohérent.
- **reserved*** : lecture exploitable si exposée ; écriture refusée ou sans effet observable ; comportement neutre et stable.

## 4. Périmètre
### Inclus
- tous les champs du mapping logique ;
- lecture ;
- écriture ;
- relecture après écriture ou tentative d’écriture ;
- cohérence type d’accès ↔ comportement observé ;
- répétabilité.

### Exclus
- contenu métier ;
- performance ;
- robustesse physique de communication.

## 5. Critères d’acceptation
- aucun champ ne viole son type d’accès ;
- les réservés restent neutres ;
- aucune divergence mapping ↔ comportement n’est observée sans justification.

## 6. Automatisation
Oui, maximale, par boucle complète sur le mapping.

## 7. Conclusion
FT-ACC-07 constitue le test de vérité final du mapping unifié.
