# S7-I2 — Navigation contextuelle TR2 et conformité des actions

## Statut

Tranche d'alignement de l'IHM réelle S7 avec la baseline UX gelée S6.

## Objet

S7-I2 corrige deux écarts de présentation constatés après S7-I1 :

1. le détail TR2 exposait séparément `État système` et `Configuration`, alors que S6 gèle une navigation contextuelle unique `État & configuration` ;
2. l'IHM exposait un bouton d'acquittement global, alors que S6 interdit explicitement tout `Acquitter tout`.

## Décisions

La navigation contextuelle TR2 devient exactement :

```text
Synthèse
Vibrations
État & configuration
Commandes
Campagnes
Diagnostic
```

La vue `État & configuration` conserve des sous-sections distinctes pour B1, B2 et B4. Cette fusion est uniquement navigationnelle et ne fusionne aucune autorité domaine ou protocole.

## Acquittement

L'IHM n'expose plus d'action globale d'acquittement. Seul l'acquittement ciblé reste présenté : saisie explicite d'un `faultCode`, `AcknowledgeFault` avec `acknowledgeAll=false`, confirmation UX avant soumission.

Le contrat backend générique n'est pas modifié dans cette tranche ; S7-I2 retire uniquement l'exposition opérateur interdite par S6.

## Invariants préservés

- navigateur sans accès Modbus direct ;
- moteur B5 unique ;
- aucune nouvelle commande B5 ;
- `Ambiguous` reste bloquant ;
- aucun Retry / Ignore / Force ;
- `RESET_STATISTICS` absent ;
- clé B5 protégée invisible du navigateur ;
- B6 reste séparé de B5 ;
- aucun changement de la spécification V1 ou du firmware.
