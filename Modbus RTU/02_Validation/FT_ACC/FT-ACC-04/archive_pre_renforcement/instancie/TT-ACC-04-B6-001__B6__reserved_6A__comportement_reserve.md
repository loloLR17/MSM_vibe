# TT-ACC-04-B6-001 — Bloc 6 — reserved_6A

## Objectif
Vérifier que le champ réservé `reserved_6A` présente un comportement neutre, stable et sans sémantique cachée.

## Exigence(s) couverte(s)
- FT-ACC-04
- Gouvernance `reserved*`
- Mapping unifié logique TR2

## Référence mapping
- Adresse début : 6010
- Adresse fin : 6011
- Type déclaré : uint16\[2]
- Nombre de registres : 2
- Accès documentaire : RO
- Description : Réservé

## Préconditions
- FT-STR validée
- FT-ACC-01 validée
- Accès Modbus opérationnel
- Simulateur en état stable
- Valeur initiale lisible

## Données d’entrée
- Adresse de départ : 6010
- Longueur : 2
- Valeur de tentative : motif de test borné à la plage réservée

## Scénario / étapes
1. Lire exactement `2` registre(s) à partir de l’adresse `6010` et mémoriser la valeur initiale.
2. Relire la même plage pour vérifier la stabilité read → read à état constant.
3. Tenter une écriture bornée à la plage `6010..6011`.
4. Contrôler le comportement de la tentative : exception Modbus explicite ou acceptation sans modification observable.
5. Relire exactement `2` registre(s) à partir de l’adresse `6010`.
6. Vérifier qu’aucune modification illégitime n’est observée et qu’aucune sémantique cachée n’apparaît.

## Résultat attendu
- comportement déterministe ;
- lecture stable ;
- tentative d’écriture refusée ou sans effet observable ;
- aucune modification illégitime du réservé.

## Critères d’acceptation
- valeur stable avant/après ;
- aucune altération du champ réservé ;
- cohérence avec la gouvernance `reserved*` ;
- aucune divergence documentaire non justifiée.

## Mode d’exécution
- simulateur déterministe
- automatisable

## Traces à conserver
- trame de lecture initiale ;
- trame de relecture ;
- trame d’écriture éventuelle ;
- valeur initiale ;
- valeur finale ;
- verdict.

## Niveau de criticité
P0
