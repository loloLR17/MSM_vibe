# FT-LIM-02 — Procédure d'exécution

## 1. Principe

Les instances sont portées par `FT-LIM-02_instancie_index.csv`. Il n'est pas nécessaire de créer une fiche Markdown répétitive par instance.

## 2. Baseline

Utiliser une configuration préparée B complète, connue valide, dérivée d'une configuration active A valide.

Avant chaque essai :
- restaurer B ;
- utiliser un nouvel identifiant de configuration si nécessaire ;
- conserver un snapshot de 4E ;
- acquisition arrêtée ;
- utiliser une transaction Bloc 5 neuve pour toute demande d'application.

Après modification de 4B+4C+4D, recalculer et écrire `prepared_config_crc` avant la demande d'application.

## 3. Exécution LIM02-G01

Pour chaque ligne C01 :

1. écrire `sampling_frequency_hz`, `window_size_samples` et `indicator_period_ms` indiqués ;
2. vérifier que chaque valeur appartient à son domaine FT-LIM-01 ;
3. calculer `lhs = indicator_period_ms × sampling_frequency_hz` ;
4. calculer `rhs = 1000 × window_size_samples` ;
5. vérifier `lhs >= rhs` ;
6. recalculer le CRC préparé ;
7. demander l'application via la commande Bloc 5 code 1 selon la procédure de commande applicable ;
8. vérifier que la relation C01 n'est pas la cause d'un refus ;
9. avec une baseline sans autre défaut, vérifier que l'application aboutit et que 4E reflète la combinaison.

Si l'étape 5 échoue pour une ligne construite uniquement avec des valeurs V1 valides, arrêter la campagne : contradiction normative à traiter avant verdict firmware.

## 4. Exécution LIM02-G02

### 4.1 Caractérisation

Avant les instances C02, établir `C`, capacité utilisable en MB, avec une preuve traçable. Enregistrer :
- valeur C ;
- source/moyen de détermination ;
- version firmware ;
- support/configuration de stockage concerné.

Si C est inconnu ou non fiable : marquer les trois instances `N/A — ENVIRONMENT_NOT_CHARACTERIZED`.

### 4.2 Valeurs compatibles

Pour `1` et `C` lorsque leurs conditions sont satisfaites :

1. restaurer B ;
2. écrire `storage_limit_mb` ;
3. vérifier l'acceptation Modbus ;
4. recalculer le CRC ;
5. demander l'application ;
6. vérifier que la contrainte de capacité ne provoque pas d'échec ;
7. avec toutes les autres préconditions satisfaites, vérifier l'activation cohérente de la valeur.

### 4.3 Valeur C+1

Si `C < 4294967295` :

1. restaurer B et snapshot A ;
2. écrire `C+1` dans `storage_limit_mb` ;
3. vérifier que l'écriture n'est pas rejetée uniquement pour la contrainte métier ;
4. recalculer le CRC ;
5. demander l'application ;
6. vérifier que la configuration n'est pas appliquée ;
7. vérifier que 4E reste identique à A ;
8. vérifier l'absence d'application partielle.

Si `C = 4294967295`, marquer `TT-LIM-02-C02-003` `N/A — NO_REPRESENTABLE_VALUE_ABOVE_C`.

## 5. Résultats

Pour chaque instance enregistrer au minimum :
- PASS / FAIL / N/A ;
- valeurs réellement écrites ;
- lhs/rhs pour C01 ;
- C et sa provenance pour C02 ;
- résultat de la commande d'application ;
- état final de configuration ;
- preuve que l'image active est correcte ou inchangée selon le cas.

Le code résultat exact ne constitue un critère obligatoire que lorsqu'une source normative le fixe sans ambiguïté pour la cause testée.
