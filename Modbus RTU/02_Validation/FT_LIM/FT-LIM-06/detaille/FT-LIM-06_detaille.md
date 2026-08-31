# FT-LIM-06 — Cas génériques

## LIM06-G01 — Domaine campaign_state
Sur une campagne sélectionnée valide, lire 6020. PASS si valeur dans {0,1,2,3,4,5}. Toute autre valeur est hors domaine V1.

## LIM06-G02 — Domaine data_integrity_status
Lire 6057. PASS si valeur dans {0,1,2,3}.

## LIM06-G03 — Domaine storage_health_status
Lire 6009. PASS si valeur dans {0,1,2,3}.

## LIM06-G04 — campaign_id non nul
Précondition : `selected_campaign_valid=1`. Lire 6012-6013. PASS si uint32 != 0.

## LIM06-G05 — Campagne en cours
Précondition : campagne valide observée avec `campaign_state=2`. Lire 6018-6019. PASS si `end_timestamp=0`.

## LIM06-G06 — Campagne non en cours
Précondition : campagne valide avec état différent de 2. La V1 ne permet pas de conclure génériquement que `end_timestamp` doit être non nul pour tous les autres états. Test documentaire : ne pas créer d’oracle supplémentaire.

## LIM06-G07 — Cohérence duration/timestamps
La V1 exige une cohérence mais ne fournit pas un oracle numérique suffisamment précis pour un verdict autonome. Consigner `start_timestamp`, `end_timestamp` et `duration_s` à des fins de traçabilité ; classer ce contrôle `NOT_TESTABLE_PRECISELY` tant que le référentiel ne définit pas la formule/tolérance attendue. Ne jamais transformer une appréciation subjective de « contradiction manifeste » en FAIL normatif.

## LIM06-G08 — Parcours de l’inventaire
Si N>0, parcourir les indices valides selon FT-LIM-05 et appliquer G01, G02 et G04 à chaque campagne exposée. G03 est global au stockage et n’est pas multiplié artificiellement par campagne.

## LIM06-G09 — États observables non provoquables
Pour une valeur enum normative qui ne peut pas être provoquée de manière sûre, documenter « non observée » ou N/A ; ne jamais écrire les registres RO ni injecter un état artificiel non spécifié.
