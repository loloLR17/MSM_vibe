# FT-LIM-07 — Procédure d’exécution

1. Lire les champs Bloc 7 sans aucune écriture directe.
2. Vérifier les domaines de 7001, 7006 et 7011.
3. Vérifier `(system_fault_flags & 0xFC00)==0`.
4. Lire `uptime_s` de manière cohérente selon FT-STR, attendre un intervalle observable, puis relire.
5. S’assurer qu’aucun reset n’a été observé entre les lectures avant de conclure sur la monotonie.
6. Si un reset a réellement eu lieu, documenter la baisse éventuelle d’uptime comme autorisée et relever `reset_cause`.
7. Si `last_fault_code=0`, appliquer uniquement la sémantique « aucun défaut connu ».
8. Pour `last_fault_timestamp`, utiliser la base temporelle Bloc 2 ; classer le contrôle TRACE_ONLY si aucune référence événementielle fiable n’est disponible.
9. Consigner les champs sans domaine normatif sans fabriquer de seuil.

## États non observables
Les états enum qui ne peuvent pas être provoqués sans perturber ou dégrader la plateforme sont notés non observés/N/A. La conformité du domaine est vérifiée sur toutes les observations disponibles.
