# FT-ACC-05 — Fiche de spécification

## Absence d’effets de bord non spécifiés

- **ID** : FT-ACC-05
- **Criticité** : P0

## Objectif
Valider qu’une écriture Modbus autorisée sur une zone `RW` ne produit aucun effet de bord non prévu par la spécification V1.

## Définition
Un effet de bord non spécifié est toute modification de registre ou d’état interne qui n’est ni :
1. la modification de la cible explicitement écrite ;
2. un effet fonctionnel explicitement prescrit par la V1 pour cette écriture ;
3. une évolution autonome du système démontrée comme indépendante de l’écriture testée.

La conformité ne signifie donc pas « seul le registre ciblé peut changer ».

## Références et hiérarchie
1. spécifications V1 gelées ;
2. mapping unifié GEL-MAP-V1 ;
3. présente source ;
4. cas `detaille/` ;
5. cas `instancie/`.

FT-STR est gelée. FT-ACC-02 valide l’autorisation d’écriture ; FT-ACC-05 valide uniquement l’absence d’effets non spécifiés après une écriture autorisée.

## Stratégie
- établir avant l’essai la liste des effets V1 autorisés pour la cible ;
- capturer un état de référence suffisant avant écriture ;
- effectuer une écriture nominale maîtrisée ;
- capturer l’état après stabilisation adaptée au comportement concerné ;
- comparer les états et classer chaque différence : cible, effet V1 autorisé, évolution autonome démontrée, ou effet non spécifié.

Le snapshot bloc complet est recommandé lorsqu’il est pertinent, mais il ne constitue pas à lui seul le critère de verdict.

## Règles particulières V1
### Bloc 2
L’écriture de `prepared_time` prépare l’heure mais ne doit pas synchroniser l’horloge par elle-même. La synchronisation effective relève d’une commande dédiée.

### Bloc 4
Toute modification d’un champ de configuration préparée peut repositionner `config_state` à `BROUILLON` conformément à la V1. Cet effet est autorisé. Une simple écriture préparée ne doit pas appliquer implicitement la configuration ni modifier l’image active.

### Bloc 5
Les champs de requête sont testés sans front de `submit`. Leur préparation ne doit pas déclencher de commande. `cmd_request_control` est testé avec une valeur non déclenchante (`0x0000`) ; les effets d’un `submit`, `cancel` ou `clear` effectif relèvent des tests fonctionnels de commande.

### Bloc 6
L’écriture de `selected_campaign_index` peut légitimement mettre à jour la vue RO de la campagne sélectionnée. Ces changements sont autorisés s’ils correspondent à la campagne demandée ; toute modification sans lien avec la sélection est interdite.

## Critères d’acceptation
- écriture Modbus autorisée et exécutée conformément à FT-ACC-02 ;
- cible modifiée conformément à la sémantique V1 ;
- seuls les effets V1 explicitement autorisés sont observés hors cible ;
- aucune action implicite interdite n’est déclenchée ;
- aucun effet non spécifié n’est observé ;
- comportement déterministe dans des préconditions équivalentes.

## Hors périmètre
- validation des droits RO/réservés ;
- validité métier des valeurs ;
- validation ou application fonctionnelle d’une configuration ;
- exécution fonctionnelle des commandes du Bloc 5 ;
- comportement structurel déjà couvert par FT-STR.
