# FT-LIM-04 — Vue d'ensemble des instances

## Synthèse
FT-LIM-04 couvre le domaine fonctionnel du moteur de commandes Bloc 5, sans refaire FT-LIM-03.

- 16 cas génériques ;
- 38 instances actives/conditionnelles ;
- commandes 1 à 11 couvertes ;
- domaine réservé du code commande couvert ;
- transaction ID, idempotence et front `submit` couverts ;
- confirmation 10/11 couverte ;
- annulation couverte sans inventer une liste de commandes annulables ;
- `clear_request_fields` couvert.

## Doctrine d'exécution
Les cas marqués `CONDITIONAL` ne deviennent exécutables que si l'environnement permet de provoquer proprement la précondition concernée. Sinon ils sont documentés `N/A` avec motif explicite.

Les essais destructifs ou perturbateurs (reset logiciel, RAZ statistiques, défauts, SD absente, mémoire insuffisante) ne sont jamais forcés sur une plateforme non dédiée.

## Frontières avec les autres familles
- FT-ACC : droits d'accès et réservés du mapping ;
- FT-LIM-03 : CRC, validation et copie prepared→active ;
- FT-LIM-04 : valeurs de commande, paramètres, soumission, préconditions et résultats fonctionnels.

## Points non définis conservés hors tests obligatoires
- masque de sous-tests autotest non nul ;
- liste exhaustive des commandes annulables ;
- détails `cmd_result_detail` ;
- temporisations exactes et durée d'exposition des états.

## Critère de gel
FT-LIM-04 est gelable si chaque exigence V1 du Bloc 5 liée aux valeurs/préconditions possède soit une instance active, soit une instance conditionnelle explicitement justifiée, sans comportement inventé et sans doublon avec FT-LIM-03/FT-ACC.
