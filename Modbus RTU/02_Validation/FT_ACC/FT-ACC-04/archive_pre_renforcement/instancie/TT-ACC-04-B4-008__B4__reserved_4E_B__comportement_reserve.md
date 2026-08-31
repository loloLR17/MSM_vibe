# TT-ACC-04-B4-008 — Bloc 4 — reserved_4E_B

## Objectif
Vérifier que le champ réservé `reserved_4E_B` présente un comportement neutre, stable et sans sémantique cachée.

## Exigence(s) couverte(s)
- FT-ACC-04
- Gouvernance `reserved*`
- Mapping unifié logique TR2

## Référence mapping
- Adresse début : 4123
- Adresse fin : 4127
- Type déclaré : uint16[5]
- Nombre de registres : 5
- Accès documentaire : RO
- Description : Réservé, doit lire 0

## Préconditions
- FT-STR validée
- FT-ACC-01 validée
- Accès Modbus opérationnel
- Simulateur en état stable
- Valeur initiale lisible

## Données d’entrée
- Adresse de départ : 4123
- Longueur : 5
- Valeur de tentative : motif de test borné à la plage réservée

## Scénario / étapes
1. Lire exactement `5` registre(s) à partir de l’adresse `4123` et mémoriser la valeur initiale.
2. Relire la même plage pour vérifier la stabilité read → read à état constant.
3. Tenter une écriture bornée à la plage `4123..4127`.
4. Contrôler le comportement de la tentative : exception Modbus explicite ou acceptation sans modification observable.
5. Relire exactement `5` registre(s) à partir de l’adresse `4123`.
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
