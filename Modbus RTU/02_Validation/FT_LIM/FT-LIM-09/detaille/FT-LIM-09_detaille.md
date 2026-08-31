# FT-LIM-09 — Cas génériques

## LIM09-G01 — Domaine time_status
PASS si valeur lue ∈ {0,1,2,3,4}.

## LIM09-G02 — Bits réservés time_flags
PASS si `(time_flags & 0xFF00)==0`.

## LIM09-G03 — Domaine prepared_time_status
PASS si valeur lue ∈ {0,1,2,3}.

## LIM09-G04 — Domaine sync_source
PASS si valeur lue ∈ {0,1,2,3,4}.

## LIM09-G05 — Écriture préparée sans effet immédiat
Relever `current_time`, écrire une nouvelle valeur préparée valide, puis relire `current_time` sans soumettre commande 2. PASS si aucune bascule vers la valeur préparée n’est observée ; l’écoulement normal du temps reste autorisé.

## LIM09-G06 — Préparé stable avant application
Après préparation et avant commande 2, vérifier que l’état préparé est exposé conformément au Bloc 2 et que l’horloge courante n’a pas été remplacée par la valeur préparée.

## LIM09-G07 — Application effective par commande 2
Avec les préconditions FT-LIM-04 satisfaites, soumettre commande 2 et observer le sous-système temporel. PASS si une synchronisation effective est observable et cohérente avec la valeur préparée, sans imposer une précision non spécifiée.

## LIM09-G08 — Monotonie hors synchronisation
Deux lectures successives sans synchronisation effective doivent satisfaire `current_time(t2) >= current_time(t1)`.

## LIM09-G09 — Discontinuité lors d’une synchronisation
Lors d’une synchronisation effective, une correction de `current_time` ne constitue pas une violation de monotonie. Le test vérifie que la discontinuité coïncide avec l’événement de synchronisation.

## LIM09-G10 — last_sync_time inchangé sans synchronisation
Sur une période sans synchronisation effective, PASS si `last_sync_time` reste inchangé.

## LIM09-G11 — last_sync_time après synchronisation
Après synchronisation effective, PASS si `last_sync_time` est mis à jour de façon cohérente avec cet événement. Ne pas imposer d’égalité exacte non spécifiée.

## LIM09-G12 — Cohérence time_since_sync
Après une synchronisation effective, observer plusieurs lectures. Vérifier que l’indicateur reflète qualitativement/monotoniquement l’ancienneté croissante depuis la synchronisation, hors nouvelle synchronisation. Toute égalité exacte ou tolérance arbitraire est interdite.

## LIM09-G13 — Grandeurs sans limites métier
Consigner `time_accuracy_ms`, `drift_ppm` et relations non spécifiées en TRACE_ONLY/NOT_DEFINED.
