# Projet MSM — Capteur de vibration TR2

## P9-L — Arbitrage firmware du périmètre statistique V1

## 1. Statut

La présente tranche **P9-L** fige l'arbitrage préalable à toute implémentation de la commande B5 **11 — RESET_STATISTICS**.

Ce document ne modifie pas la spécification Modbus RTU V1. Il explicite uniquement ce que l'état courant du firmware permet ou ne permet pas d'implémenter sans inventer une nouvelle autorité métier.

---

## 2. Référence normative V1

La commande B5 `RESET_STATISTICS` est définie par la V1 comme une remise à zéro des **statistiques de service non critiques**.

La V1 impose en outre :

- `confirm_key = 0xA55A` ;
- `param1 = 0` tant qu'aucun masque futur n'est défini ;
- ne jamais effacer les campagnes ;
- ne jamais effacer l'identité capteur ;
- ne jamais effacer la configuration ;
- ne jamais effacer les journaux critiques.

La V1 ne fournit pas de catalogue exhaustif des statistiques de service concernées.

---

## 3. Architecture gelée

L'architecture firmware distingue explicitement :

```text
runtime pure counters
≠ persistent statistics
≠ critical histories
```

Elle impose également que :

- les statistiques de service restent distinctes des historiques critiques ;
- un `StatisticsStore` n'existe que si son périmètre est défini ultérieurement ;
- le périmètre exact des statistiques persistantes et de `RESET_STATISTICS` reste volontairement ouvert.

La classification transactionnelle gelée indique qu'aucun `CommandRecoveryContext` additionnel B5 n'est requis pour `RESET_STATISTICS`.

---

## 4. Inventaire firmware P9-L

À l'état courant de `main`, aucune autorité de statistiques métier n'existe :

- aucun `StatisticsService` ;
- aucun `StatisticsStore` ;
- aucun snapshot statistique métier ;
- aucun record persistant de statistiques ;
- aucun catalogue de compteurs resettable par la commande 11.

Les éléments présents suivants ne sont **pas** requalifiés en statistiques resettable :

- `uptime_s`, dérivé du `MonotonicClock` du boot courant ;
- générations de snapshots/services ;
- `recovered_campaign_count` et inventaire B6 ;
- métadonnées et historique de campagnes ;
- `last_fault_code` / `last_fault_timestamp` ;
- dernier résultat SELFTEST ;
- état d'acquittement des défauts ;
- états runtime acquisition/campagne ;
- compteur/état transactionnel du `CommandJournal` ;
- instrumentation de tests ou compteurs d'appels HAL host.

Ces objets possèdent chacun une autre sémantique ou une autre autorité et ne doivent pas être remis à zéro par assimilation implicite.

---

## 5. Arbitrage P9-L

### 5.1 Périmètre statistique implémentable aujourd'hui

```text
EMPTY
```

Aucune statistique de service réelle n'est actuellement définie comme cible de `RESET_STATISTICS`.

### 5.2 Conséquence

La commande B5 `11 — RESET_STATISTICS` **ne doit pas être raccordée à un succès métier vide**.

En particulier, sont interdits :

- un `StatisticsService` factice dont l'unique effet serait de retourner `TR2_OK` ;
- un `StatisticsStore` vide créé uniquement pour rendre la commande exécutable ;
- la remise à zéro de `uptime_s` ;
- la remise à zéro de générations internes de snapshots ;
- l'effacement d'historiques diagnostiques ;
- l'effacement du `CommandJournal` ;
- l'effacement de campagnes ou de leur inventaire ;
- la transformation de compteurs de tests/HAL en statistiques métier.

### 5.3 Classification

- existence de la commande 11 et ses restrictions : **normatif V1** ;
- séparation statistiques / historiques critiques : **architecture gelée** ;
- périmètre exact des statistiques persistantes : **NOT_DEFINED V1 / architecture volontairement ouverte** ;
- constat qu'aucune autorité statistique réelle n'existe dans le firmware courant : **état d'implémentation P9-L** ;
- décision de ne pas implémenter un succès no-op : **FW_POLICY de rigueur conforme à l'architecture**.

---

## 6. Condition de réouverture

Une future implémentation de `RESET_STATISTICS` exige d'abord une décision explicite définissant au minimum :

1. la liste exacte des statistiques de service ;
2. leur propriétaire métier ;
3. leur caractère volatile ou persistant ;
4. leur unité et leur sémantique ;
5. leur politique de saturation/wrap si applicable ;
6. leur comportement au boot ;
7. la frontière atomique d'une remise à zéro ;
8. les exclusions critiques ;
9. la projection éventuelle de ces statistiques dans une interface publique ;
10. les tests de fault injection associés si une persistance est introduite.

Tant que cette décision n'existe pas, `RESET_STATISTICS` reste volontairement **non connectable** dans le firmware V1 courant.

---

## 7. Conséquence pour P9

P9-L clôt l'arbitrage statistique sans créer de nouvelle autorité métier.

La tranche **P9-M — StatisticsService + RESET_STATISTICS** n'est donc pas applicable à la baseline courante et doit être sautée tant qu'un périmètre statistique réel n'est pas défini.

La suite pertinente de P9 devient la passe d'intégration runtime des commandes réellement raccordables, puis la passe transverse de recovery/idempotence et le gel P9.
