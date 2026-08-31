# FT-LIM-10 — Vue d’ensemble

## Couverture
16 cas génériques et 17 instances.

## Oracles directs
- domaines de statut, sévérité, axe dominant, dépassements et alarme mémorisée ;
- bits réservés des deux bitfields ;
- monotonie des compteurs hors reset/RAZ ;
- saturation autorisée ;
- réserves 3040..3047 à zéro ;
- convention V1 d’accélération en mg.

## Oracles conditionnels
- rattachement des seuils à la configuration active ;
- absence de rétroactivité d’une configuration seulement préparée ;
- prise d’effet à la prochaine fenêtre validée après activation ;
- qualification correcte d’une dernière valeur conservée.

## Garde-fous
Aucune relation exhaustive entre statuts, flags, sévérité, dépassements ou alarmes n’est fabriquée. Aucun seuil de fraîcheur, plage RMS/crête ou relation algébrique global/axes n’est ajouté sans base normative.

La cohérence structurelle du snapshot reste référencée à FT-STR.
