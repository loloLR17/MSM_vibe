# FT-CMD-07 — Source normative consolidée

## 1. ENTER / EXIT MAINTENANCE
Commande 8 active le mode maintenance. Commande 9 le désactive.

La V1 indique pour ENTER MAINTENANCE une **politique recommandée** : accepter uniquement si l'acquisition est arrêtée, sinon refus explicite. Cette phrase n'est pas une condition normative d'acceptation ; aucun code de refus obligatoire ne doit être fabriqué à partir de cette recommandation.

## 2. Commandes protégées
`cmd_request_confirm_key` est utilisé uniquement pour les commandes protégées.

Valeurs définies :
- `0x0000` : aucune confirmation ;
- `0xA55A` : confirmation valide.

Commandes protégées en V1 :
- 10 RESET SOFTWARE ;
- 11 RESET STATISTICS.

`cmd_result_code = 9` signifie commande protégée / confirmation absente.

Aucune protection par clé ne doit être étendue aux commandes 1 à 9.

## 3. RESET SOFTWARE
Conditions d'acceptation :
- acquisition arrêtée ;
- aucune opération critique non terminée.

Effets normatifs annoncés : journalisation, accusé de prise en compte, redémarrage logiciel contrôlé.

Refus typiques explicitement donnés :
- `9` confirmation absente ;
- `5` acquisition en cours.

La V1 ne donne pas de code résultat précis pour le cas « opération critique non terminée » et ne définit pas exhaustivement ce qu'est une opération critique.

Le comportement après redémarrage et les règles de persistance appartiennent à FT-PER.

## 4. RESET STATISTICS
La clé `0xA55A` est obligatoire. `param1` est un masque éventuel futur, sinon `0`.

Effet : remise à zéro des statistiques de service non critiques.

Non-effets explicitement normés : ne pas effacer les campagnes, l'identité capteur, la configuration ni les journaux critiques.

La liste exacte des statistiques réinitialisées n'est pas exhaustive en V1 ; ne pas inventer de masque ni de liste complète.

## 5. Dettes candidates V1.1
- rendre normative ou supprimer la recommandation d'entrée en maintenance acquisition arrêtée ;
- définir la notion d'opération critique et son code de refus ;
- définir précisément le périmètre de RESET STATISTICS et l'usage éventuel de `param1`.
