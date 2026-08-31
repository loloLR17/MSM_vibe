# FT-BLK-03 — Cas de test détaillés

## TT-BLK-B03-201 — Calcul RMS global
- **Objectif** : vérifier la convention RMS de la norme vectorielle.
- **Source** : Bloc 3 §5.1 et §5.3.
- **Préconditions** : banc capable d'injecter/rejouer une fenêtre X/Y/Z déterministe ; fenêtre validée.
- **Entrées** : séquence connue `(x_i, y_i, z_i)` en accélération.
- **Étapes** : calculer indépendamment `sqrt(mean(x_i²+y_i²+z_i²))` selon les règles numériques applicables ; injecter la fenêtre ; lire `B3_RMS_GLOBAL_MG`.
- **Résultat attendu** : résultat compatible avec l'oracle indépendant.
- **Critère d'acceptation** : écart conforme à la règle d'arrondi/quantification normative ou au plan de banc ; aucune tolérance arbitraire n'est créée ici.
- **Mode** : conditionnel, automatisable avec banc déterministe.
- **Criticité** : P0.

## TT-BLK-B03-202 — Calcul crête globale
- **Objectif** : vérifier le maximum de norme vectorielle.
- **Source** : Bloc 3 §5.1 et §5.3.
- **Préconditions** : banc déterministe.
- **Étapes** : calculer indépendamment `max(sqrt(x_i²+y_i²+z_i²))`, injecter la fenêtre, lire `B3_PEAK_GLOBAL_MG`.
- **Résultat attendu** : résultat compatible avec l'oracle indépendant selon les règles numériques applicables.
- **Mode** : conditionnel.
- **Criticité** : P0.

## TT-BLK-B03-203 — Calcul RMS par axe
- **Objectif** : vérifier `B3_RMS_X_MG`, `B3_RMS_Y_MG`, `B3_RMS_Z_MG`.
- **Source** : Bloc 3 §5.2 et §5.3.
- **Préconditions** : banc déterministe.
- **Étapes** : calculer indépendamment le RMS de chaque axe, injecter la fenêtre, lire les trois résultats.
- **Résultat attendu** : chaque axe est compatible avec son oracle indépendant.
- **Mode** : conditionnel.
- **Criticité** : P0.

## TT-BLK-B03-204 — Calcul crête par axe
- **Objectif** : vérifier les maxima absolus X/Y/Z.
- **Source** : Bloc 3 §5.2 et §5.3.
- **Préconditions** : banc déterministe.
- **Étapes** : calculer `max(abs(x_i))`, `max(abs(y_i))`, `max(abs(z_i))`, injecter la fenêtre et comparer aux champs exposés.
- **Résultat attendu** : résultats compatibles avec les oracles indépendants.
- **Mode** : conditionnel.
- **Criticité** : P0.

## TT-BLK-B03-205 — Conservation conditionnelle de la dernière valeur
- **Objectif** : vérifier la politique de qualification si l'implémentation choisit de conserver la dernière valeur calculée lors d'une indisponibilité temporaire.
- **Source** : Bloc 3 §6.
- **Préconditions** : une valeur valide existe ; le firmware implémente la conservation autorisée ; indisponibilité temporaire reproductible.
- **Étapes** : relever les valeurs valides ; provoquer l'indisponibilité sans reset ; relire le Bloc 3.
- **Résultat attendu** : si les valeurs sont conservées, elles ne sont pas présentées comme un nouveau calcul valide/frais et les états de qualification reflètent cette situation sans contradiction avec les définitions normatives.
- **Critère d'acceptation** : aucune nouvelle valeur fictive ; qualification cohérente avec la politique effectivement implémentée.
- **Mode** : conditionnel.
- **Criticité** : P1.

## TT-BLK-B03-206 — Indication LAST_VALUE_HELD
- **Objectif** : vérifier l'indication explicite d'une dernière valeur conservée.
- **Source** : Bloc 3 §6 et §8.2 bit 9.
- **Préconditions** : scénario où une dernière valeur est effectivement conservée sans recalcul récent.
- **Étapes** : provoquer le scénario ; lire `B3_VALIDITY_FLAGS`.
- **Résultat attendu** : `LAST_VALUE_HELD=1` pendant l'état correspondant.
- **Critère d'acceptation** : le bit signale la situation qu'il définit normativement.
- **Mode** : conditionnel.
- **Criticité** : P1.

## TT-BLK-B03-207 — Stabilité de B3_CALC_SEQUENCE sans nouvelle fenêtre
- **Objectif** : vérifier qu'aucune nouvelle séquence n'est imputée en l'absence de nouvelle fenêtre validée.
- **Source** : Bloc 3 mapping et §9.
- **Préconditions** : capacité à maintenir le système dans un état où aucune nouvelle fenêtre de calcul n'est validée, sans reset.
- **Étapes** : lire `B3_CALC_SEQUENCE`, attendre dans cet état, relire le compteur.
- **Résultat attendu** : valeur inchangée.
- **Critère d'acceptation** : aucune progression sans nouvelle fenêtre validée.
- **Mode** : automatisable sous précondition maîtrisée.
- **Criticité** : P1.
- **Limite** : la monotonie générale du compteur est déjà couverte par FT-BLK-02.

## TT-BLK-B03-208 — Monotonie de B3_EXCEED_COUNT
- **Objectif** : vérifier que le compteur total de dépassements ne régresse pas en fonctionnement sans reset.
- **Source** : Bloc 3 §9.
- **Préconditions** : aucun reset pendant le test.
- **Étapes** : lire le compteur à plusieurs instants couvrant idéalement au moins un événement de dépassement.
- **Résultat attendu** : `count[n+1] >= count[n]`.
- **Critère d'acceptation** : aucune diminution observée.
- **Mode** : automatisable.
- **Criticité** : P1.
- **Limite** : le lien événement B4/B3 → incrément est traité séparément ; ce test ne fixe pas la politique de saturation.

## TT-BLK-B03-209 — Monotonie de B3_ALARM_COUNT
- **Objectif** : vérifier que le compteur total d'alarmes ne régresse pas en fonctionnement sans reset.
- **Source** : Bloc 3 §9.
- **Préconditions** : aucun reset pendant le test.
- **Étapes** : lire le compteur à plusieurs instants couvrant idéalement au moins une alarme.
- **Résultat attendu** : `count[n+1] >= count[n]`.
- **Critère d'acceptation** : aucune diminution observée.
- **Mode** : automatisable.
- **Criticité** : P1.
- **Limite** : aucune exigence de wrap/saturation supplémentaire n'est inventée.
