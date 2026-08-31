# FT-INT-04 — Référentiel source

## 1. Sources normatives

- `01_Specification_source/bloc1.md`
- `01_Specification_source/bloc2.md`
- `01_Specification_source/bloc5.md`
- `01_Specification_source/bloc6.md`

Sources de délégation principales :
- `FT_BLK/FT-BLK-05_Inventaire_campagnes/`
- future famille `FT-CMD`
- future famille `FT-PER`

## 2. Inventaire des relations

| ID | Relation inter-blocs | Statut | Propriétaire / remarque |
|---|---|---|---|
| INT04-R01 | Succès B5 START → acquisition B1 effectivement active | COVERED | FT-INT-04 |
| INT04-R02 | Succès B5 START → ouverture d'une nouvelle campagne B6 | COVERED | FT-INT-04 |
| INT04-R03 | Campagne ouverte après START cohérente avec l'état « en cours » B6 | COVERED | FT-INT-04 ; invariants internes B6 restent FT-BLK-05 |
| INT04-R04 | Succès B5 STOP → acquisition B1 arrêtée | COVERED | FT-INT-04 |
| INT04-R05 | STOP → campagne précédemment ouverte mise en cohérence / clôturée | COVERED | Oracle borné à ce que V1 impose explicitement |
| INT04-R06 | Timestamps de campagne B6 utilisent la base temporelle B2 | COVERED | FT-INT-04 |
| INT04-R07 | `B1.active_campaign_id == B6.campaign_id` de la campagne courante | NOT_DEFINED | Ne pas déduire l'égalité de la sémantique des noms |
| INT04-R08 | START → `total_campaign_count` augmente exactement de 1 immédiatement | NOT_DEFINED | Ouverture normée, timing/incrément observable strict non normé |
| INT04-R09 | RAZ statistiques B5 ne supprime pas les campagnes B6 | COVERED | FT-INT-04 |
| INT04-R10 | RAZ statistiques ne supprime pas identité capteur / configuration | COVERED | Contrôle transversal associé ; ne pas étendre à la persistance reboot |
| INT04-R11 | Refus START/STOP et codes 21/22 et autres préconditions | DELEGATED | FT-CMD |

## 3. Oracles normatifs retenus

### 3.1 Démarrage acquisition

La commande B5 code `3` a pour effets normatifs :
- démarrage de l'acquisition ;
- ouverture d'une nouvelle campagne selon la logique firmware ;
- mise à jour de l'état système.

Le Bloc 1 définit `acquisition_state = 1` comme « en cours ».

### 3.2 Arrêt acquisition

La commande B5 code `4` a pour effets normatifs :
- arrêt propre de l'acquisition ;
- vidage des buffers ;
- fermeture des fichiers ;
- mise en cohérence de la campagne en cours.

Le Bloc 1 définit `acquisition_state = 0` comme « arrêtée ».

### 3.3 Campagne en cours

Le Bloc 6 définit :
- `campaign_state = 2` : en cours ;
- `campaign_state = 3` : terminée ;
- `end_timestamp = 0` si campagne en cours ;
- `campaign_id != 0` pour une campagne valide.

Les deux derniers invariants sont propriétaires FT-BLK-05 lorsqu'ils sont testés isolément. FT-INT-04 peut les utiliser comme éléments de preuve de la campagne créée par START.

### 3.4 Base temporelle

Le Bloc 6 impose que `start_timestamp` et `end_timestamp` utilisent la base temporelle du Bloc 2.

L'oracle FT-INT-04 vérifie une appartenance cohérente à cette base dans des fenêtres de temps bornées. Il n'impose pas l'égalité exacte entre valeurs lues à des instants différents.

### 3.5 RAZ statistiques

La commande B5 code `11` remet à zéro des statistiques de service non critiques et ne doit pas effacer :
- les campagnes ;
- l'identité capteur ;
- la configuration ;
- les journaux critiques.

FT-INT-04 porte la vérification inter-blocs immédiate de conservation des campagnes. Les propriétés de persistance à travers un redémarrage appartiennent à FT-PER.

## 4. Anti-fabrication

### 4.1 Identifiant de campagne actif

B1 expose `active_campaign_id` et B6 expose `campaign_id`, mais la V1 ne formule pas explicitement l'égalité normative entre ces champs. La relation reste `NOT_DEFINED`.

### 4.2 Compteur de campagnes

L'ouverture d'une nouvelle campagne est normative. En revanche, la V1 ne fixe pas un délai de publication ni une règle observable stricte imposant `total_campaign_count_après = total_campaign_count_avant + 1` dès la première lecture. Cette égalité n'est donc pas utilisée comme oracle obligatoire.

### 4.3 Durée

B6 exige une durée cohérente avec les timestamps mais précise qu'elle peut être recalculée firmware. FT-BLK-05 a déjà conservé l'égalité exacte universelle `duration_s = end_timestamp - start_timestamp` comme insuffisamment définie. FT-INT-04 respecte cette limite.

## 5. Délégation FT-CMD

Les scénarios de refus, notamment :
- START sans configuration active valide ;
- START acquisition déjà active ;
- STOP acquisition inactive ;
- SD absente ;
- mémoire insuffisante ;
- défaut critique actif ;
- codes résultat `21`, `22` ;
- répétition du même `transaction_id` ;

ne sont pas des tests FT-INT-04. Ils seront traités par FT-CMD.