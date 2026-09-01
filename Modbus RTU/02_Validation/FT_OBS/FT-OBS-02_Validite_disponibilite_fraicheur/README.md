# FT-OBS-02 — Validité, disponibilité et fraîcheur des données

## 1. Objet

FT-OBS-02 vérifie qu'une centrale peut déterminer, **sans heuristique**, si une information exposée est exploitable lorsque la V1 fournit explicitement les discriminants nécessaires.

La sous-famille ne redéfinit ni les domaines de valeurs, ni les sentinelles, ni la cohérence temporelle : elle vérifie l'usage déterministe des indicateurs normatifs de validité et de fraîcheur.

## 2. Oracle V1 principal

Deux mécanismes sont suffisamment définis :

- **Bloc 2 — Temps** : `time_status` et `time_flags` permettent de qualifier explicitement la validité et l'état de synchronisation de la base de temps ;
- **Bloc 3 — Supervision vibratoire** : `B3_STATUS_GLOBAL`, `B3_VALIDITY_FLAGS`, `B3_VALUE_AGE_MS` et `B3_LAST_UPDATE_TR2` permettent de distinguer valeur valide, fraîche, dégradée, invalide et dernière valeur conservée.

La V1 dit explicitement que les valeurs B3 peuvent rester présentes même lorsqu'elles ne sont plus fraîches. Une valeur numérique non nulle n'est donc jamais, à elle seule, une preuve de validité ou de fraîcheur.

## 3. Principe d'exploitation

La centrale doit donner priorité aux métadonnées normatives de qualification :

- `VALUES_VALID=1` qualifie la validité des valeurs B3 ;
- `VALUES_FRESH=1` qualifie leur fraîcheur ;
- `LAST_VALUE_HELD=1` indique qu'une dernière valeur est conservée sans recalcul récent ;
- `DATA_DEGRADED=1` signale des données dégradées mais potentiellement utilisables ;
- `CALC_ERROR=1` signale une erreur de calcul ;
- `time_flags.TIME_VALID` et `time_status` qualifient la base de temps.

FT-OBS-02 n'invente aucune règle supplémentaire de combinaison lorsque la V1 ne la donne pas.

## 4. Frontières de propriété

Sont `DELEGATED` :

- domaines des enums, bits réservés, encodage et uint32 : FT-STR / FT-LIM ;
- monotonie, cohérence des timestamps et relations `last_sync_time` / `time_since_sync` : FT-BLK / FT-INT ;
- cohérence de calcul B3 et snapshots : FT-BLK / FT-INT ;
- seuil exact séparant « frais » et « périmé » : aucun seuil numérique n'est défini par FT-OBS ;
- campagnes B6 : discriminabilité de l'entrée sélectionnée déjà traitée par FT-OBS-01, autres propriétés dans FT-OBS-04.

## 5. Points explicitement NOT_DEFINED

La V1 ne définit pas de sentinelle globale signifiant « donnée absente », « inconnue » ou « invalide » pour tous les registres.

Elle ne permet donc pas de généraliser :

- `0 = absent` ;
- `0 = invalide` ;
- `0xFFFF = inconnu` ;
- `0xFFFFFFFF = indisponible`.

Toute sentinelle éventuelle reste strictement locale au champ qui la définit explicitement.

La V1 ne définit pas non plus un âge numérique universel au-delà duquel toute donnée devient périmée. Le bit `VALUES_FRESH` est l'oracle d'exploitation B3 ; `B3_VALUE_AGE_MS` est une mesure d'âge, pas un seuil inventé par FT-OBS.

## 6. Tests actifs

- `TT-OBS-B02-001` — décodage de validité et synchronisation de la base de temps ;
- `TT-OBS-B03-001` — qualification valide/fraîche/dégradée/invalide via statut et flags B3 ;
- `TT-OBS-B03-002` — dernière valeur conservée : une valeur présente ne doit pas être interprétée comme fraîche par heuristique.

## 7. Artefacts

- `source/FT-OBS-02_source.md` ;
- `detaille/FT-OBS-02_detaille.md` ;
- `detaille/FT-OBS-02_matrice_couverture.csv`.

## 8. Statut

Sous-famille reconstruite pour revue. Aucun gel ni merge sans validation explicite.