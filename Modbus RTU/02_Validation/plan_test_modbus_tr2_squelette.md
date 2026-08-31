# MSM — Capteur de vibration (TR2)
# Plan de test protocole Modbus RTU
## Plan maître de validation — socle de gouvernance V1

> **Statut**
>
> Le présent document définit le cadre méthodologique de validation du protocole TR2.
> Le **socle de gouvernance** décrit ici est aligné sur la spécification Modbus RTU V1 gelée.
> Les familles de tests non encore développées restent à détailler ; leur contenu futur ne pourra pas contredire la spécification normative.

---

## 1. Objet du document

Ce document définit l’architecture macro du plan de test du protocole Modbus RTU du système TR2.

Il couvre :

- l’architecture globale du plan de test ;
- la hiérarchie documentaire ;
- la logique de validation ;
- la structuration officielle des familles de tests ;
- les risques couverts ;
- l’ordre d’exécution recommandé ;
- les principes d’automatisation et de simulation ;
- les règles de gestion des ambiguïtés et divergences.

Il ne remplace ni la spécification Modbus RTU V1 ni les fiches détaillées des familles et cas de test.

---

## 2. Référentiel et contexte technique

### 2.1 Hiérarchie documentaire gelée

La hiérarchie suivante s’applique à toute la validation TR2 :

1. **Spécification Modbus RTU V1 gelée** : `01_Specification_source/bloc0.md` à `bloc7.md` et `charte_typage.md` ;
2. **mapping unifié**, artefact dérivé de la spécification ;
3. fiches source des familles et sous-familles de validation ;
4. cas de test détaillés génériques ;
5. cas de test instanciés à partir du mapping.

Le mapping unifié constitue la **source opérationnelle d’instanciation** des tests, mais pas une norme indépendante.

En cas de divergence entre un artefact dérivé et la spécification V1, la spécification fait foi. La divergence doit être enregistrée comme anomalie documentaire ou de spécification ; elle ne doit jamais être corrigée silencieusement en modifiant la V1.

### 2.2 Système visé

Le système TR2 est un système industriel de capteurs de vibration :

- capteurs autonomes en mode **store-and-forward** ;
- acquisition locale ;
- stockage local sur SD ;
- communication filaire **Modbus RTU sur RS-485** ;
- aucune dépendance à une liaison permanente pendant une campagne ;
- centrale chargée notamment de la configuration, synchronisation temporelle, supervision, diagnostic et consultation des données exposées.

### 2.3 Architecture protocolaire

Le protocole est organisé en huit blocs :

- **Bloc 0** — Identification ;
- **Bloc 1** — État système ;
- **Bloc 2** — Temps ;
- **Bloc 3** — Supervision vibratoire ;
- **Bloc 4** — Configuration acquisition ;
- **Bloc 5** — Commandes ;
- **Bloc 6** — Inventaire campagnes ;
- **Bloc 7** — Diagnostic.

### 2.4 Choix techniques gelés par la V1

- types protocolaires autorisés : `uint16`, `int16`, `uint32`, `bitfield16`, `enum16`, `ASCII fixe` ;
- `uint16[n]` peut être utilisé uniquement comme notation documentaire de regroupement ;
- aucun float ;
- un registre Modbus transporte exactement 16 bits ;
- `uint32` = **MSW puis LSW** ;
- chaînes ASCII fixes : deux caractères par registre, padding `0x00` ;
- cohérence multi-registres à un **même instant logique** ;
- une sentinelle telle que `0 = non renseigné` n’existe que lorsqu’elle est explicitement définie pour le champ concerné ;
- les accès invalides et les valeurs métier invalides sont deux notions distinctes.

### 2.5 Doctrine gelée des accès invalides

Une requête Modbus est invalide lorsqu’elle vise une adresse inexistante ou effectue une opération interdite par la spécification ou le mapping dérivé conforme, notamment une écriture sur un registre RO ou réservé.

Une requête invalide doit :

- générer une exception Modbus standard appropriée ;
- ne modifier aucun registre ni état interne ;
- ne jamais être exécutée partiellement ;
- produire un comportement déterministe.

Une lecture d’un **sous-ensemble valide** n’est pas invalide du seul fait qu’elle est partielle.

Une valeur métier hors domaine écrite dans un registre explicitement RW ne constitue pas, à elle seule, un accès Modbus invalide. Son traitement relève des règles fonctionnelles du bloc et des familles métier/limites.

---

## 3. Périmètre de validation

### 3.1 Inclus

La validation protocolaire couvre :

- conformité du mapping dérivé à la spécification ;
- conformité structurelle et sémantique des blocs ;
- droits d’accès ;
- cohérence inter-blocs ;
- transactions et séquences métier ;
- valeurs limites et domaines invalides ;
- robustesse protocolaire ;
- persistance et reprise ;
- observabilité et exploitabilité ;
- validation sur simulateur avant disponibilité du matériel réel.

### 3.2 Hors périmètre de cette phase protocolaire

- justesse métrologique vibratoire ;
- performances physiques du MEMS ;
- robustesse électrique réelle du bus RS-485 ;
- perturbations CEM ;
- endurance matérielle réelle de la SD ;
- essais environnementaux ;
- performances non définies par une exigence protocolaire.

Ces sujets pourront faire l’objet de plans de validation distincts.

---

## 4. Architecture globale du plan de test

Le plan est structuré comme une validation en couches.

### Niveau A — Base protocolaire et structurelle

Vérifier adresses, tailles, types, encodages, droits d’accès, réservés, sentinelles explicitement définies et comportement aux frontières.

### Niveau B — Fonctionnel intra-bloc

Valider chaque bloc isolément selon sa sémantique normative.

### Niveau C — Cohérence inter-blocs

Vérifier les dépendances croisées et l’absence de contradiction entre blocs.

### Niveau D — Transactionnel et séquentiel

Valider préparation, commande, transition, acquittement et état final.

### Niveau E — Robustesse et résilience protocolaire

Valider répétitions, accès dégradés, lectures pendant transitions, pertes de réponse et scénarios de resynchronisation.

### Niveau F — Persistance et reprise

Vérifier ce qui est conservé, réinitialisé ou reconstruit après reboot ou coupure simulée, lorsque ces comportements sont spécifiés.

### Niveau G — Exploitabilité système

Vérifier qu’une centrale peut interpréter l’interface sans heuristique ni hypothèse implicite.

---

## 5. Stratégie globale de validation

Principes :

- validation **incrémentale** ;
- validation du structurel vers le métier ;
- nominal avant limites ;
- limites avant robustesse ;
- robustesse avant persistance ;
- non-régression après chaque correction ;
- toute ambiguïté normative est traitée avant propagation aux familles aval.

Étapes générales :

1. audit documentaire statique ;
2. validation du mapping dérivé ;
3. validation sur simulateur déterministe ;
4. automatisation progressive ;
5. injection de fautes ;
6. validation système orientée exploitation ;
7. reprise ultérieure sur matériel réel lorsque disponible.

---

## 6. Structure formelle du référentiel de validation

Le référentiel doit comporter au minimum :

1. référentiel normatif ;
2. gouvernance ;
3. mapping dérivé ;
4. familles de tests ;
5. matrice de couverture ;
6. critères d’entrée et de sortie ;
7. gestion des anomalies ;
8. moyens de test ;
9. traces et rapports ;
10. automatisation.

L’arborescence détaillée est définie dans `00_gourvernance/CHARTE_ARBORESCENCE.md`.

---

## 7. Convention d’identification des tests

Convention générale :

`TT-<famille>-<scope>-<numéro>`

Exemples :

- `TT-STR-GEN-001` ;
- `TT-ACC-B04-003` ;
- `TT-BLK-B02-011` ;
- `TT-INT-B03B04-004` ;
- `TT-CMD-B05-007` ;
- `TT-SEQ-SYS-012` ;
- `TT-PER-SYS-005`.

Conventions :

- `GEN` = transversal/générique ;
- `SYS` = système ;
- `B00` à `B07` = bloc ciblé ;
- un scope composite peut être utilisé pour les tests inter-blocs.

Les conventions plus spécifiques d’une famille peuvent préciser ce format sans casser la traçabilité globale.

---

## 8. Format standard d’une fiche de test

Chaque cas de test détaillé doit comporter au minimum :

- identifiant ;
- titre ;
- objectif ;
- exigence(s) ou règle(s) couverte(s) ;
- source normative ;
- préconditions ;
- données d’entrée ;
- étapes ;
- résultat attendu ;
- critères d’acceptation ;
- mode d’exécution ;
- automatisation possible ;
- traces à conserver ;
- criticité ;
- remarques, limites et éventuels arbitrages.

Aucun résultat attendu ne doit être inventé lorsque la spécification ne définit pas le comportement : le cas est alors marqué **NON DÉFINI / À ARBITRER**.

---

## 9. Matrice de couverture

### 9.1 Objectif

Assurer la traçabilité entre :

- exigences et règles normatives ;
- risques ;
- familles ;
- cas de test ;
- statut de validation.

### 9.2 Colonnes minimales recommandées

- `Req_ID` ;
- `Bloc` ;
- `Intitulé` ;
- `Source` ;
- `Type` ;
- `Risque_ID` ;
- `Priorité` ;
- `Famille_Test` ;
- `Test_ID` ;
- `Auto` ;
- `Statut` ;
- `Remarques`.

---

## 10. Priorisation et anomalies

### 10.1 Priorité des tests

- **P0** — critique protocole ;
- **P1** — critique exploitation ;
- **P2** — important mais contournable ;
- **P3** — confort / lisibilité.

### 10.2 Classification des anomalies

- **A1** — bloquante ;
- **A2** — majeure ;
- **A3** — mineure ;
- **A4** — observation ;
- **AS** — ambiguïté ou contradiction de spécification.

Une anomalie AS bloquant une règle aval doit être arbitrée avant génération ou validation des tests qui en dépendent.

---

## 11. Table officielle des familles de tests

### 11.1 FT-STR — Conformité structurelle

**Objectif :** vérifier la conformité binaire et structurelle de l’exposition Modbus.

**Périmètre :** adresses, tailles, types, MSW/LSW, ASCII, réservés, sentinelles explicitement définies, cohérence multi-registres.

**Priorité :** P0.

La décomposition FT-STR-01 à FT-STR-08 est définie dans `FT_STR/Specifications.md`. Pour la V1, **FT-STR-05 = Réservés et sentinelles** et **FT-STR-06 = Accessibilité lecture**.

### 11.2 FT-ACC — Accès et permissions

**Objectif :** vérifier les droits d’accès et l’absence d’effet de bord non prévu.

**Périmètre :** lecture des zones exposées, écriture RW, refus RO, refus d’écriture sur réservés, accès hors plage, absence d’effet de bord.

**Priorité :** P0.

### 11.3 FT-BLK — Fonctionnel intra-bloc

**Objectif :** valider la cohérence fonctionnelle interne de chaque bloc.

**Périmètre :** blocs 0 à 7, valeurs, états, enums, bitfields, règles fonctionnelles et sentinelles définies.

**Priorité :** P0/P1 selon le bloc et la fonction.

### 11.4 FT-INT — Cohérence inter-blocs

**Objectif :** vérifier l’absence de contradiction entre blocs et la cohérence des dépendances croisées.

**Périmètre typique :** B1↔B7, B2↔B5, B4↔B5, B4↔B3, B1↔B3, B1↔B6, B6↔B7.

**Priorité :** P0/P1.

### 11.5 FT-LIM — Valeurs limites et domaines invalides

**Objectif :** vérifier le comportement aux frontières et sur les valeurs métier hors domaine.

**Périmètre :** min/max, zéro lorsqu’il a une signification définie, sentinelles explicites, enums hors domaine, index invalides, paramètres incohérents.

**Règle critique :** une valeur métier invalide dans un registre RW valide n’est pas automatiquement un accès Modbus invalide.

**Priorité :** P0/P1.

### 11.6 FT-CMD — Moteur de commandes

**Objectif :** valider le comportement transactionnel du Bloc 5 et les actions qu’il pilote.

**Périmètre :** soumission, transaction ID, prise en compte, refus, exécution, statut, résultat, répétition et unicité d’exécution.

**Priorité :** P0.

### 11.7 FT-SEQ — Séquences métier

**Objectif :** valider les enchaînements réalistes d’exploitation par la centrale.

**Périmètre :** qualification initiale, configuration, synchronisation temporelle, consultation état/inventaire/diagnostic, maintenance et autres scénarios explicitement permis par la V1.

**Priorité :** P1, certaines séquences pouvant être P0.

### 11.8 FT-RBT — Robustesse protocolaire

**Objectif :** valider le comportement sous défauts de communication ou séquencement dégradé.

**Périmètre :** répétitions, pertes de réponse, requêtes invalides, écritures incomplètes au sens transactionnel, lectures pendant transition, timeouts logiques et resynchronisation.

**Priorité :** P0/P1.

### 11.9 FT-PER — Persistance et reprise

**Objectif :** vérifier les comportements spécifiés après reboot ou coupure simulée.

**Périmètre :** identité, configurations, campagnes, diagnostic, état système, temps et cohérence post-reprise lorsque les exigences normatives existent.

**Priorité :** P0.

Les comportements de persistance non définis dans la spécification doivent être marqués **NON DÉFINI / À ARBITRER** et non supposés.

### 11.10 FT-OBS — Observabilité et exploitabilité

**Objectif :** vérifier que le protocole fournit une vision exploitable par une centrale industrielle.

**Périmètre :** lisibilité des états, distinction défaut actif/mémorisé, donnée absente/invalide/zéro réel lorsqu’elle est spécifiée, qualité des codes et aptitude au diagnostic distant.

**Priorité :** P1.

---

## 12. Vue synthétique des familles

| Famille | But principal | Portée | Priorité |
|---|---|---|---|
| FT-STR | conformité binaire | structure protocolaire | P0 |
| FT-ACC | contrôle des droits | accès registres | P0 |
| FT-BLK | validité locale | bloc isolé | P0/P1 |
| FT-INT | non-contradiction | multi-blocs | P0/P1 |
| FT-LIM | robustesse de domaine | valeurs frontières | P0/P1 |
| FT-CMD | sûreté transactionnelle | commandes | P0 |
| FT-SEQ | usage réel | scénarios métier | P1 |
| FT-RBT | tolérance aux défauts | protocole dégradé | P0/P1 |
| FT-PER | reprise et stockage | reboot/persistance | P0 |
| FT-OBS | lisibilité exploitation | supervision/maintenance | P1 |

---

## 13. Dépendances entre familles

Dépendances fortes :

- FT-STR avant les autres familles ;
- FT-ACC avant les validations métier utilisant des zones RW ;
- FT-BLK avant FT-INT ;
- FT-BLK + FT-ACC avant FT-CMD ;
- FT-CMD avant les séquences qui utilisent les commandes ;
- FT-RBT après validation nominale ;
- FT-PER seulement sur des règles de persistance normativement définies ou après arbitrage ;
- FT-OBS après disponibilité des scénarios nécessaires.

FT-LIM peut être développée progressivement dès que les domaines normatifs correspondants sont clairement définis.

---

## 14. Ordre d’exécution recommandé

### Phase 1 — Base protocolaire

1. FT-STR
2. FT-ACC

### Phase 2 — Validation bloc par bloc

3. FT-BLK / Bloc 0
4. FT-BLK / Bloc 1
5. FT-BLK / Bloc 2
6. FT-BLK / Bloc 4
7. FT-BLK / Bloc 5
8. FT-BLK / Bloc 6
9. FT-BLK / Bloc 7
10. FT-BLK / Bloc 3

### Phase 3 — Cohérence, domaines et usage

11. FT-INT
12. FT-LIM
13. FT-CMD
14. FT-SEQ

### Phase 4 — Résilience et exploitation

15. FT-RBT
16. FT-PER
17. FT-OBS

Cet ordre est un ordre de validation recommandé ; il n’interdit pas la préparation documentaire anticipée d’une famille aval.

---

## 15. Architecture du banc de test

Le banc de validation doit pouvoir comporter :

- client Modbus de test ;
- simulateur de capteur ;
- moteur de scénarios ;
- journal d’échanges ;
- moteur d’assertions ;
- générateur de rapports.

Modes de simulation souhaités :

- nominal déterministe ;
- défaut injecté ;
- incohérence volontaire pour validation négative ;
- redémarrage simulé ;
- persistance simulée lorsque la règle testée est définie.

Les observations doivent distinguer : valeur brute registre, valeur décodée, état métier simulé, transition d’état et verdict.

---

## 16. Architecture du simulateur capteur

Le simulateur doit être structuré en couches :

### Couche 1 — Modèle métier

Identité, temps, état système, configuration préparée/active, campagnes, défauts, diagnostics et commandes.

### Couche 2 — Projection registre

Transformation déterministe de l’état métier en image Modbus conforme à la V1.

### Couche 3 — Moteur d’événements

Transitions et fautes contrôlées nécessaires aux scénarios.

### Couche 4 — Persistance simulée

Conservation sélective uniquement selon les règles normatives ou arbitrages documentés.

Les profils d’état historiques (`S00_INIT`, `S01_IDLE_SYNCED`, etc.) restent des **outils de simulation** et ne deviennent pas des états protocolaires normatifs sauf intégration explicite à la spécification.

---

## 17. Risques principaux

- **R1** — erreur de mapping binaire ;
- **R2** — contradiction de sémantique ;
- **R3** — effet implicite non voulu ;
- **R4** — séquence transactionnelle ambiguë ;
- **R5** — perte de cohérence après coupure ;
- **R6** — persistance incorrecte ;
- **R7** — supervision non exploitable ;
- **R8** — angle mort ou ambiguïté de spécification.

---

## 18. Doctrines de gouvernance — état de gel

### Doctrine 1 — Non renseigné / zéro réel / non applicable / invalide

**GELÉ partiellement par la V1.**

`0` ne signifie « non renseigné » que si le champ le définit explicitement. Pour `int16`, zéro est une valeur physique valide sauf convention explicite contraire. Les autres distinctions doivent être documentées champ par champ.

### Doctrine 2 — Cohérence / atomicité logique des lectures

**GELÉ pour les réponses Modbus multi-registres.**

Les registres retournés dans une même lecture doivent correspondre à un même instant logique, conformément à `charte_typage.md`.

L’atomicité métier d’une commande ou d’une bascule de configuration relève des règles normatives des blocs concernés et doit être testée selon celles-ci.

### Doctrine 3 — Persistance normative

**À ARBITRER au cas par cas lorsque la V1 ne définit pas le comportement.**

FT-PER ne doit pas inventer ce qui survit au reboot ou à une coupure.

### Doctrine 4 — Répétition de commande

**À DÉRIVER du Bloc 5 et à arbitrer si une situation n’y est pas définie.**

Aucune règle d’idempotence ne doit être supposée sans source normative.

### Doctrine 5 — Cohérence temporelle

**À DÉRIVER du Bloc 2 et des dépendances explicites des autres blocs.**

Les comportements non définis en cas de temps invalide ou non synchronisé sont marqués **NON DÉFINI / À ARBITRER**.

### Doctrine 6 — Accès Modbus invalide vs valeur métier invalide

**GELÉ par la V1.**

Une opération interdite ou une adresse inexistante relève d’une exception Modbus. Une valeur hors domaine écrite dans un registre RW valide relève de la logique fonctionnelle du bloc.

---

## 19. Suite de travail

Ordre de consolidation recommandé après le gel de gouvernance :

1. audit croisé de `mapping_unifie` contre la spécification V1 ;
2. audit complet et gel de FT-STR ;
3. audit complet et gel de FT-ACC ;
4. reprise de FT-LIM sur les domaines effectivement définis ;
5. développement progressif de FT-BLK, FT-INT, FT-CMD, FT-SEQ, FT-RBT, FT-PER et FT-OBS ;
6. mise en place de l’automatisation et du simulateur sur la base des artefacts gelés.

À chaque étape : analyser → exposer l’écart → proposer → arbitrer → corriger/régénérer → auditer → geler.

---

## 20. Conclusion

Le présent plan définit le cadre de validation industriel du protocole Modbus RTU TR2.

Son socle repose sur :

- une spécification V1 normative gelée ;
- des artefacts dérivés explicitement subordonnés à cette spécification ;
- des familles de tests orientées risques ;
- une séparation stricte entre structure, accès, métier, limites, robustesse et persistance ;
- une traçabilité des ambiguïtés au lieu d’hypothèses implicites ;
- une architecture compatible avec simulation et automatisation.

Les éléments non définis par la V1 restent **NON DÉFINI / À ARBITRER** jusqu’à décision formelle.
