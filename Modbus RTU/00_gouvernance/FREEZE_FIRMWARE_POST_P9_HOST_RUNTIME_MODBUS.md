# Projet MSM — Capteur de vibration TR2

## Gel firmware Post-P9 — Clôture Host / runtime / frontières Modbus V1

## 1. Statut

La passe **Post-P9 — clôture Host/runtime/frontières Modbus V1**, tranches A1 à A7, est considérée comme implémentée et validée sur la branche `main`.

Baseline fonctionnelle immédiatement avant le présent gel :

- commit : `e934eaac782df42c937ae52a32a9dacbe058f70d` ;
- commit : `Firmware: align Post-P9-A7 read validation` ;
- validation locale utilisateur : `./tr2_validate.sh` ;
- résultat : **67/67 tests réussis** ;
- runtime Host : validation canonique verte.

Le présent document est un constat de gel d'implémentation. Il ne modifie pas la spécification Modbus RTU V1, ne remplace pas les documents d'architecture gelés et ne transforme aucune zone `NOT_DEFINED V1`, décision `FW_POLICY` ou extension V1.1 en exigence normative V1.

---

## 2. Baseline antérieure

Cette passe part du gel P9 :

- `FREEZE_FIRMWARE_P9_DIAGNOSTIC_COMMANDS_RUNTIME.md` ;
- commit de gel P9 : `04e2be1ff975f0bbfeb6a922a34e01cc5374828f` ;
- dernière baseline fonctionnelle P9 avant gel : `05beff4b23889dc21c5dc406c58fb576ad8af309` ;
- validation P9 : **66/66 tests réussis**.

Tous les invariants P1 à P9 restent applicables.

---

## 3. Objet de la passe Post-P9

La passe Post-P9 ferme les frontières logicielles Host/runtime/Modbus qui restaient incomplètes après P9, sans introduire de plateforme STM32 ni de comportement matériel fictif.

Elle couvre :

```text
A1  B4 read path
A2  runtime read images B1/B3
A3  frontière de lecture B0
A4  audit/fermeture des écritures V1 B2/B4/B5/B6
A5  intégration Host de lecture B0…B7
A6  complétion du Register Model B5/B6/B7
A7  alignement des adapters sur le Register Model central
```

---

## 4. Register Model V1

À l'issue de A6/A7, le `Register Model` central décrit les huit blocs V1 B0 à B7 et constitue l'autorité structurelle commune pour la validation des plages de lecture et d'écriture.

Les adapters ne maintiennent plus de seconde classification structurelle B5/B6/B7 concurrente.

Chaîne gelée :

```text
Register Model B0…B7
        ↓
validate_read / validate_write
        ↓
adapters
        ↓
projection / effet métier
```

Les validations sémantiques propres à un champ restent dans l'adapter métier concerné. En particulier, B5 conserve la validation des bits autorisés de `cmd_request_control`.

---

## 5. Frontière de lecture Modbus

`ModbusReadSources` consomme désormais des images runtime déjà projetées pour B0, B1, B3, B4, B5, B6 et B7, et un `TimeSnapshot` pour B2.

Le read adapter :

1. valide la plage via le Register Model central ;
2. vérifie que la plage reste dans un seul bloc ;
3. distribue vers B0…B7 ;
4. ne reconstruit pas opportunément B1/B3/B4/B5/B6/B7 pendant une lecture.

Une source absente reste explicitement `TR2_ERROR_NOT_AVAILABLE` ; aucune donnée fictive n'est créée pour rendre une lecture disponible.

---

## 6. Disponibilité B2 et B3 au boot Host

La validation Post-P9 gèle explicitement le comportement observé au boot Host :

- B2 possède une source `TimeSnapshot`, mais sa projection peut rester `TR2_ERROR_NOT_AVAILABLE` tant que les faits de synchronisation et l'heure courante requis ne sont pas disponibles ;
- B3 reste `TR2_ERROR_NOT_AVAILABLE` tant qu'aucun vrai snapshot de supervision n'a été produit ;
- aucune heure, mesure vibratoire ou supervision artificielle n'est fabriquée pour satisfaire le test d'intégration.

Les sorties de lecture ne sont pas artificiellement modifiées lorsqu'une projection est indisponible.

---

## 7. Frontière B0 / identité

A3 ferme la frontière Modbus de B0 : le read adapter consomme une `ModbusBlock0Image` déjà projetée et ne reprojette plus directement un `IdentitySnapshot` brut à chaque lecture.

Cette décision ne définit pas le mécanisme physique de provisioning de l'identité.

Restent explicitement ouverts :

- l'affectation initiale du `device_id` ;
- la source physique du numéro de série ;
- la source exacte des informations produit non reconstructibles ;
- la politique globale de boot/READY si l'identité provisionnée est absente, corrompue ou indisponible ;
- la composition définitive `IdentityService -> SystemRuntime -> ModbusBlock0Image` sur la plateforme cible.

Conformément à l'architecture gelée, aucune identité n'est inventée ni régénérée silencieusement.

Le test Host A5 utilise uniquement une identité explicite de fixture de test ; elle ne constitue ni une valeur par défaut runtime ni une politique de provisioning.

---

## 8. Frontières d'écriture V1

La passe A4/A7 confirme les chemins suivants :

- B2 : validation structurelle centrale puis sémantique temporelle existante ;
- B4 : validation structurelle centrale puis staging/configuration existants ;
- B5 : validation structurelle centrale, validation sémantique du contrôle, mailbox et Command Engine existants ;
- B6 : validation structurelle centrale, seul `selected_campaign_index` est RW, puis effet métier via `CampaignInventoryService`.

Les écritures RO, réservées, inexistantes ou invalides sont rejetées sans mutation partielle silencieuse.

Aucune écriture V1.1 n'est ajoutée.

---

## 9. Intégration Host B0…B7

A5 ajoute un test d'intégration de la chaîne :

```text
HostPlatform
→ SystemRuntime
→ sources runtime réelles disponibles
→ ModbusReadSources
→ modbus_read_adapter_read()
```

Le test couvre un représentant de chacun des blocs B0 à B7.

Au boot Host testé :

```text
B0 -> OK via fixture d'identité explicitement projetée
B1 -> OK depuis SystemRuntime
B2 -> NOT_AVAILABLE selon état temporel réel du boot
B3 -> NOT_AVAILABLE sans snapshot réel de supervision
B4 -> OK depuis SystemRuntime
B5 -> OK depuis SystemRuntime
B6 -> OK depuis SystemRuntime
B7 -> OK depuis SystemRuntime
```

Cette intégration valide les frontières logicielles ; elle ne constitue pas un serveur Modbus RTU physique.

---

## 10. Ce que ce gel ne couvre pas

Le présent gel ne prétend pas fermer les éléments matériels ou de plateforme suivants :

- cible STM32 ;
- BSP/HAL STM32 ;
- UART physique ;
- driver RS485 / DE-RE ;
- framing/timing RTU physique sur liaison série ;
- boucle serveur Modbus RTU de production liée à l'UART ;
- `PersistentMedia` physique ;
- `VibrationSource` physique ;
- producteurs matériels de température interne et tension d'alimentation ;
- provisioning physique de l'identité B0 ;
- exécuteur SELFTEST matériel détaillé au-delà du contrat existant.

Ces éléments doivent être traités dans des tranches plateforme dédiées et ne doivent pas être simulés silencieusement dans le runtime V1.

---

## 11. Points fonctionnels volontairement ouverts

Restent également hors du présent gel :

- commande 11 `RESET_STATISTICS`, conformément à P9-L (`EMPTY`) ;
- `transaction_epoch` et autres extensions V1.1 ;
- catalogues supplémentaires de diagnostics ou SELFTEST non normatifs ;
- politiques de provisioning non définies par V1 ;
- toute extension de mapping réservée à V1.1.

---

## 12. Validation

La baseline fonctionnelle immédiatement avant ce gel a été validée localement par l'utilisateur :

```text
100% tests passed, 0 tests failed out of 67
TR2 validation complete.
```

La validation couvre la non-régression P1 à P9 ainsi que les tranches Post-P9 A1 à A7.

---

## 13. Décision de gel

À compter de ce gel :

- les frontières logicielles Host/runtime/Modbus V1 de B0 à B7 sont considérées closes dans le périmètre explicitement défini ci-dessus ;
- le Register Model central couvre B0 à B7 et constitue l'autorité structurelle d'accès ;
- les adapters lisent/écrivent conformément à cette autorité structurelle sans mapping parallèle B5/B6/B7 ;
- les images runtime restent des projections et ne deviennent pas des autorités métier ;
- les indisponibilités réelles B2/B3 ne sont pas masquées par des données fictives ;
- la frontière B0 est prête à recevoir une composition d'identité réelle, mais son provisioning et sa politique globale d'indisponibilité restent explicitement ouverts ;
- aucun serveur série physique, aucune plateforme STM32 et aucun producteur matériel absent ne sont implicitement déclarés implémentés.

La prochaine phase devra partir de cette baseline et définir explicitement la transition vers l'intégration plateforme/STM32, après vérification de l'état réel de `main`.
