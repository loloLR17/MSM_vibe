# FT-LIM-10 — Supervision vibratoire : domaines et invariants fonctionnels

## Objet
Valider les domaines normatifs et invariants fonctionnels du Bloc 3 sans dupliquer les contrôles structurels FT-STR ni les permissions FT-ACC.

## Périmètre
- enums de statut, sévérité, axe dominant et indicateurs 0/1 ;
- bits réservés des bitfields de validité et d’alarme ;
- compteurs monotones avec saturation autorisée ;
- registres réservés maintenus à zéro ;
- application des seuils issus de la configuration active à la fenêtre de calcul ;
- absence de rétroactivité d’un changement de configuration ;
- politique de conservation d’une dernière valeur et qualification explicite de validité/fraîcheur.

## Doctrine
Une valeur présente n’est pas automatiquement une valeur valide ou fraîche. Les relations non explicitement définies entre statuts, flags, sévérité, dépassements et alarmes ne sont pas inventées.
