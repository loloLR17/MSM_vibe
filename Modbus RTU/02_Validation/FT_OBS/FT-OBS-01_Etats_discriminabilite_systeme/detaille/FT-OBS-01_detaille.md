# FT-OBS-01 — Procédures détaillées

## Doctrine d'exécution

Les tests FT-OBS-01 ne cherchent pas à reproduire les scénarios métier propriétaires des familles gelées. Ils vérifient qu'une centrale ou un parseur V1 peut **associer sans ambiguïté une valeur observée à la signification normative définie**.

Les états peuvent être obtenus :
- sur simulateur par injection contrôlée d'images de registres conformes V1 ;
- sur banc en réutilisant des préconditions/scénarios déjà validés par les familles propriétaires ;
- par rejeu de captures de registres dont l'état fonctionnel est connu.

Aucun PASS FT-OBS ne vaut PASS de la transition ayant produit l'état.

---

## TT-OBS-B01-001 — États B1 interprétables sans heuristique

### Objectif

Vérifier que `storage_status`, `acquisition_state`, `system_status` et les bits définis de `system_flags` possèdent une interprétation univoque côté centrale, sans inventer de règle exhaustive de synthèse entre ces observables.

### Sources normatives

- B1 §5 mapping ;
- B1 §6.1 `system_status` ;
- B1 §6.2 `system_flags` ;
- B1 §6.6 `storage_status` ;
- B1 §6.7 `acquisition_state`.

### Jeux d'essai normatifs

Pour `system_status` :
- 0 → UNKNOWN ;
- 1 → NOMINAL ;
- 2 → DEGRADED ;
- 3 → FAULT.

Pour `system_flags` :
- bit 0 → READY ;
- bit 1 → ACQUISITION_ACTIVE ;
- bit 2 → CONFIG_VALID ;
- bit 3 → TIME_VALID ;
- bit 4 → STORAGE_AVAILABLE.

Pour `storage_status` :
- 0 → Non disponible ;
- 1 → Disponible ;
- 2 → Plein ;
- 3 → Erreur.

Pour `acquisition_state` :
- 0 → Arrêtée ;
- 1 → En cours ;
- 2 → Pause ;
- 3 → Erreur.

### Étapes

1. Présenter successivement au décodeur/à la centrale chacune des valeurs définies de `system_status` et vérifier l'interprétation exacte.
2. Présenter des motifs couvrant chacun des bits définis de `system_flags` et vérifier leur décodage ; ne donner aucune signification aux bits 5..15.
3. Présenter successivement chacune des valeurs définies de `storage_status` et vérifier leur signification V1.
4. Répéter pour `acquisition_state`.
5. Vérifier que le banc ne calcule pas une valeur attendue de `system_status` depuis `system_flags`, `storage_status`, `acquisition_state` ou un autre bloc selon une règle non spécifiée.
6. Si le moyen d'essai possède déjà une gestion générique des valeurs réservées, consigner son comportement sans le requalifier : ce contrôle appartient à FT-LIM.

### PASS

- les quatre valeurs définies de `system_status` sont décodées sans ambiguïté ;
- les cinq bits définis de `system_flags` sont interprétés conformément à B1 ;
- les quatre valeurs définies de `storage_status` sont décodées sans ambiguïté ;
- les quatre valeurs définies de `acquisition_state` sont décodées sans ambiguïté ;
- aucune règle privée de dérivation de `system_status` n'est utilisée comme oracle V1.

### FAIL

- confusion entre deux codes définis ;
- signification différente de la table V1 ;
- signification inventée pour un bit réservé de `system_flags` ;
- utilisation comme oracle V1 d'une règle de synthèse non normative pour `system_status`.

### Pas de FAIL FT-OBS-01

- un état fonctionnel n'est pas produit par le firmware : propriété de la famille fonctionnelle ;
- une valeur réservée est produite : propriété FT-LIM/FT-BLK selon le cas ;
- deux blocs ne sont pas cohérents entre eux : propriété FT-INT.

---

## TT-OBS-B04-001 — Discriminabilité de `config_state`

### Objectif

Vérifier que la centrale peut distinguer les états de configuration définis par B4 sans devoir examiner les CRC, IDs ou autres champs pour leur attribuer une signification.

### Jeux d'essai normatifs

- 0 → VIDE ;
- 1 → BROUILLON ;
- 2 → VALIDE ;
- 4 → ACTIF ;
- 5 → ERREUR_VALIDATION ;
- 6 → ERREUR_APPLICATION.

La valeur 3 et les valeurs 7..65535 sont réservées et restent propriétaires FT-LIM.

### Étapes

1. Présenter chaque code défini de `config_state` au décodeur/à la centrale.
2. Vérifier l'association exacte code ↔ état.
3. Vérifier que les états `VALIDE` et `ACTIF` restent distincts.
4. Vérifier que `ERREUR_VALIDATION` et `ERREUR_APPLICATION` restent distinctes.
5. Ne pas imposer, dans ce test, le chemin de transition ayant produit l'état.

### PASS

Chaque code défini est interprété conformément à la table normative et reste distinguable des autres codes définis.

### FAIL

- confusion de deux états ;
- conversion d'un état réservé en état métier ;
- dépendance obligatoire à une heuristique externe pour distinguer un code pourtant défini.

### Délégations

- transitions autorisées : FT-BLK / FT-CMD / FT-INT ;
- domaines/réservés : FT-LIM ;
- état après reboot : FT-PER.

---

## TT-OBS-B06-001 — États campagne et stockage interprétables

### Objectif

Vérifier qu'une centrale peut déterminer explicitement :
- si l'entrée sélectionnée est valide ;
- l'état de la campagne sélectionnée lorsqu'elle est valide ;
- le niveau de santé du stockage.

### Jeux d'essai normatifs

`selected_campaign_valid` :
- 0 → index invalide ;
- 1 → campagne valide.

`campaign_state` :
- 0 → vide ;
- 1 → en préparation ;
- 2 → en cours ;
- 3 → terminée ;
- 4 → erreur ;
- 5 → partiellement corrompue.

`storage_health_status` :
- 0 → OK ;
- 1 → warning ;
- 2 → dégradé ;
- 3 → critique.

### Étapes

1. Présenter `selected_campaign_valid = 0` avec des métadonnées arbitraires non nulles dans l'image d'essai.
2. Vérifier que la centrale marque l'entrée comme invalide et **n'utilise pas les métadonnées comme preuve d'une campagne valide**.
3. Présenter `selected_campaign_valid = 1` puis chacun des six codes définis de `campaign_state`.
4. Vérifier leur interprétation exacte et distincte.
5. Présenter chacun des quatre codes définis de `storage_health_status` et vérifier leur interprétation exacte.
6. Ne pas déduire d'une campagne `terminée` qu'elle possède automatiquement une intégrité `OK` : `data_integrity_status` est un observable distinct, traité dans le périmètre campagne/validité ultérieur.

### PASS

- `selected_campaign_valid` contrôle explicitement l'exploitabilité de l'entrée ;
- les six états campagne sont décodés sans ambiguïté ;
- les quatre niveaux de santé stockage sont décodés sans heuristique.

### FAIL

- les métadonnées sont considérées valides malgré `selected_campaign_valid = 0` ;
- confusion entre deux états campagne ou deux niveaux de santé ;
- invention d'une relation non définie entre état campagne et intégrité des données.

---

## Cas volontairement non instanciés dans FT-OBS-01

### Dérivation exhaustive de `system_status`

`NOT_DEFINED`. Aucun test ne doit imposer une formule de calcul, une priorité ou une machine d'état reliant `system_status` aux autres états/flags au-delà des règles explicitement écrites en V1.

### Cohérence acquisition B1 / campagne B6

Toute relation stricte éventuellement définie entre blocs reste à FT-INT. FT-OBS-01 ne suppose pas qu'un code local implique automatiquement un code précis dans l'autre bloc.

### Persistance des états

Hors périmètre ; propriétaire FT-PER.

### Défauts et avertissements

Reportés à FT-OBS-03 afin de traiter proprement actif / historique / mémorisé sans mélanger les owners.