# TT-ACC-02-B6-002 — Bloc 6 — plage_rw_bloc

## Objectif
Vérifier qu’une écriture multi-registres sur la plage de travail `RW` du bloc 6 est techniquement acceptée et correctement relue.

## Exigence(s) couverte(s)
- FT-ACC-02
- Mapping unifié logique TR2
- Couverture des zones `RW` du bloc 6

## Référence mapping
- Bloc : 6
- Adresse début de plage RW : 6003
- Adresse fin de plage RW : 6003
- Taille de la plage : 1 registre(s)

## Préconditions
- FT-STR validée
- FT-ACC-01 validée
- Accès Modbus opérationnel
- Simulateur en état nominal stable
- Valeurs initiales de la plage lisibles

## Données d’entrée
- Adresse de départ : 6003
- Longueur : 1
- Jeu de valeurs de test borné à la plage RW du bloc

## Scénario / étapes
1. Lire la plage `6003..6003`.
2. Écrire une séquence de test strictement bornée à cette plage.
3. Contrôler l’absence d’exception Modbus sur l’écriture.
4. Relire la même plage.
5. Vérifier la cohérence write → read sur la plage écrite.

## Résultat attendu
- l’écriture multi-registres est acceptée ;
- la relecture de la plage correspond à l’écriture réalisée ;
- aucune divergence de longueur n’est observée.

## Critères d’acceptation
- écriture réussie ;
- relecture conforme ;
- longueur conforme ;
- comportement cohérent avec la cartographie RW du bloc.

## Mode d’exécution
- simulateur déterministe
- automatisable

## Automatisation possible
Oui

## Traces à conserver
- trame de lecture initiale ;
- trame d’écriture ;
- trame de lecture de contrôle ;
- dump brut avant/après ;
- verdict.

## Niveau de criticité
P0

## Remarques / limites
- cette fiche complète les tests par champ ;
- elle ne remplace pas les validations de limites métier ni les effets de bord détaillés.
