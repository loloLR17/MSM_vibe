# FT-BLK-03 — Cas de test détaillés

> Chaque cas est autonome et suit le format du plan maître. Les traces minimales comprennent données injectées, valeurs d'oracle, lectures Modbus brutes, horodatage du banc et verdict.

## TT-BLK-B03-201 — Calcul RMS global
- **Objectif** : vérifier la convention RMS de la norme vectorielle.
- **Exigence couverte** : BLK03-B3-001.
- **Source normative** : Bloc 3 §5.1 et §5.3.
- **Préconditions** : banc capable d'injecter/rejouer une fenêtre X/Y/Z déterministe ; fenêtre validée.
- **Entrées** : séquence connue `(x_i, y_i, z_i)`.
- **Étapes** : calculer indépendamment `sqrt(mean(x_i²+y_i²+z_i²))` ; injecter la fenêtre ; lire `B3_RMS_GLOBAL_MG`.
- **Résultat attendu** : résultat compatible avec l'oracle indépendant.
- **Critère d’acceptation** : écart conforme à la règle numérique normative ou au plan de banc ; aucune tolérance arbitraire.
- **Mode d’exécution** : conditionnel sur banc déterministe.
- **Automatisation** : oui si injection/rejeu disponible.
- **Traces** : fenêtre injectée, oracle, valeur lue, trames et verdict.
- **Criticité** : P0.
- **Limites / arbitrages** : aucune règle d'arrondi non spécifiée n'est inventée.

## TT-BLK-B03-202 — Calcul crête globale
- **Objectif** : vérifier le maximum de norme vectorielle.
- **Exigence couverte** : BLK03-B3-002.
- **Source normative** : Bloc 3 §5.1 et §5.3.
- **Préconditions** : banc déterministe.
- **Entrées** : séquence X/Y/Z connue.
- **Étapes** : calculer `max(sqrt(x_i²+y_i²+z_i²))`, injecter la fenêtre, lire `B3_PEAK_GLOBAL_MG`.
- **Résultat attendu** : résultat compatible avec l'oracle indépendant.
- **Critère d’acceptation** : conformité à l'oracle selon les règles numériques applicables.
- **Mode d’exécution** : conditionnel.
- **Automatisation** : oui si banc déterministe.
- **Traces** : entrée, oracle, valeur lue et trames.
- **Criticité** : P0.
- **Limites / arbitrages** : aucune tolérance arbitraire.

## TT-BLK-B03-203 — Calcul RMS par axe
- **Objectif** : vérifier les RMS X/Y/Z.
- **Exigence couverte** : BLK03-B3-003.
- **Source normative** : Bloc 3 §5.2 et §5.3.
- **Préconditions** : banc déterministe.
- **Entrées** : séquences connues X/Y/Z.
- **Étapes** : calculer le RMS de chaque axe, injecter, lire `B3_RMS_X_MG`, `Y`, `Z`.
- **Résultat attendu** : chaque axe est compatible avec son oracle.
- **Critère d’acceptation** : trois comparaisons conformes.
- **Mode d’exécution** : conditionnel.
- **Automatisation** : oui si banc déterministe.
- **Traces** : entrées, trois oracles, trois valeurs et trames.
- **Criticité** : P0.
- **Limites / arbitrages** : pas de règle numérique supplémentaire inventée.

## TT-BLK-B03-204 — Calcul crête par axe
- **Objectif** : vérifier les maxima absolus X/Y/Z.
- **Exigence couverte** : BLK03-B3-004.
- **Source normative** : Bloc 3 §5.2 et §5.3.
- **Préconditions** : banc déterministe.
- **Entrées** : séquences connues X/Y/Z.
- **Étapes** : calculer `max(abs(x_i))`, `max(abs(y_i))`, `max(abs(z_i))`, injecter et comparer.
- **Résultat attendu** : résultats compatibles avec les oracles indépendants.
- **Critère d’acceptation** : trois comparaisons conformes.
- **Mode d’exécution** : conditionnel.
- **Automatisation** : oui si banc déterministe.
- **Traces** : entrées, oracles, valeurs et trames.
- **Criticité** : P0.
- **Limites / arbitrages** : aucune tolérance non spécifiée.

## TT-BLK-B03-205 — Conservation conditionnelle de la dernière valeur
- **Objectif** : qualifier le comportement si l'implémentation choisit de conserver la dernière valeur calculée lors d'une indisponibilité temporaire.
- **Exigence couverte** : BLK03-B3-006.
- **Source normative** : Bloc 3 §6.
- **Préconditions** : valeur valide existante ; conservation effectivement implémentée ; indisponibilité reproductible.
- **Entrées** : scénario d'indisponibilité maîtrisé.
- **Étapes** : relever les valeurs valides ; provoquer l'indisponibilité ; relire B3.
- **Résultat attendu** : si conservées, les valeurs ne sont pas présentées comme un nouveau calcul frais et la qualification reste non contradictoire.
- **Critère d’acceptation** : aucune nouvelle valeur fictive et qualification cohérente.
- **Mode d’exécution** : conditionnel à l'implémentation.
- **Automatisation** : selon capacité d'injection.
- **Traces** : état avant/après, flags, valeurs, événement injecté.
- **Criticité** : P1.
- **Limites / arbitrages** : la V1 autorise la conservation sans la rendre obligatoire.

## TT-BLK-B03-206 — Indication LAST_VALUE_HELD
- **Objectif** : vérifier l'indication explicite d'une dernière valeur conservée.
- **Exigence couverte** : BLK03-B3-007.
- **Source normative** : Bloc 3 §6 et §8.2 bit 9.
- **Préconditions** : scénario effectif de valeur conservée sans recalcul récent.
- **Entrées** : scénario maîtrisé.
- **Étapes** : provoquer le scénario ; lire `B3_VALIDITY_FLAGS`.
- **Résultat attendu** : `LAST_VALUE_HELD = 1` pendant la situation correspondante.
- **Critère d’acceptation** : concordance entre situation et bit.
- **Mode d’exécution** : conditionnel.
- **Automatisation** : selon banc.
- **Traces** : scénario, flags et trames.
- **Criticité** : P1.
- **Limites / arbitrages** : non applicable si l'implémentation ne conserve pas la dernière valeur.

## TT-BLK-B03-207 — Stabilité de B3_CALC_SEQUENCE sans nouvelle fenêtre
- **Objectif** : vérifier l'absence de nouvelle séquence sans nouvelle fenêtre validée.
- **Exigence couverte** : BLK03-B3-008.
- **Source normative** : Bloc 3 mapping et §9.
- **Préconditions** : état contrôlé sans nouvelle fenêtre validée et sans reset.
- **Entrées** : scénario de maintien sans nouvelle fenêtre.
- **Étapes** : lire `B3_CALC_SEQUENCE`, attendre, relire.
- **Résultat attendu** : valeur inchangée.
- **Critère d’acceptation** : aucune progression.
- **Mode d’exécution** : sous précondition maîtrisée.
- **Automatisation** : oui si le banc contrôle l'absence de nouvelle fenêtre.
- **Traces** : deux valeurs, durée, état du banc.
- **Criticité** : P1.
- **Limites / arbitrages** : la monotonie générale est couverte par FT-BLK-02.

## TT-BLK-B03-208 — Monotonie de B3_EXCEED_COUNT
- **Objectif** : vérifier que le compteur total de dépassements ne régresse pas hors reset.
- **Exigence couverte** : BLK03-B3-009.
- **Source normative** : Bloc 3 §9.
- **Préconditions** : aucun reset pendant le test.
- **Entrées** : aucune ; idéalement scénario incluant un dépassement.
- **Étapes** : lire plusieurs fois `B3_EXCEED_COUNT`.
- **Résultat attendu** : `count[n+1] >= count[n]`.
- **Critère d’acceptation** : aucune diminution.
- **Mode d’exécution** : fonctionnel intra-bloc.
- **Automatisation** : oui.
- **Traces** : série horodatée des valeurs.
- **Criticité** : P1.
- **Limites / arbitrages** : pas de politique de saturation supplémentaire imposée.

## TT-BLK-B03-209 — Monotonie de B3_ALARM_COUNT
- **Objectif** : vérifier que le compteur total d'alarmes ne régresse pas hors reset.
- **Exigence couverte** : BLK03-B3-010.
- **Source normative** : Bloc 3 §9.
- **Préconditions** : aucun reset pendant le test.
- **Entrées** : aucune ; idéalement scénario incluant une alarme.
- **Étapes** : lire plusieurs fois `B3_ALARM_COUNT`.
- **Résultat attendu** : `count[n+1] >= count[n]`.
- **Critère d’acceptation** : aucune diminution.
- **Mode d’exécution** : fonctionnel intra-bloc.
- **Automatisation** : oui.
- **Traces** : série horodatée des valeurs.
- **Criticité** : P1.
- **Limites / arbitrages** : aucune exigence de wrap/saturation non explicite n'est ajoutée.
