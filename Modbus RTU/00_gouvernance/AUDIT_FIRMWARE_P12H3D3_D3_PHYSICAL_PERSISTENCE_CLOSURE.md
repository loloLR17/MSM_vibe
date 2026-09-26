# Audit P12-H3d3-D3 — Clôture persistance physique et mesures de caractérisation

## Statut

**CADRAGE — À IMPLÉMENTER**

Baseline d'entrée :

`312ae583e8c03edb58300a64b74f476ac281aba0` — `Governance: freeze P12-H3d3-D2-D physical power-loss qualification`

D2-D est gelé. D3 ne réouvre ni H3d2 ni les résultats D2-A à D2-D.

## 1. Objet

Fermer les éléments de H3d3 relatifs à la persistance physique qui restent réellement non démontrés après D2-D, sans transformer les limites explicites du gel power-loss en exigences implicites.

D3 est une tranche de **caractérisation et clôture**, pas une nouvelle conception du protocole transactionnel.

## 2. Acquis qui ne doivent pas être rejoués

Sont déjà acquis et gelés :

- NUCLEO-U575ZI-Q et STM32U575 opérationnels ;
- FRAM MB85RS2MTA identifiée et accessible en SPI ;
- PhysicalStorage brut lecture/écriture qualifié ;
- recovery H3d2 sur FRAM réelle ;
- format physique explicite ;
- commit transactionnel réel ;
- persistance après reboot/reflash ;
- quatre frontières power-loss D2-D1 à D2-D4 ;
- séparation H3d2 portable / backend STM32 conservée.

D3 ne doit donc pas reformater la FRAM ni répéter les coupures D2-D.

## 3. Écarts restants du cadrage H3d3-0

Le cadrage initial demandait encore des preuves qui ne sont pas couvertes par les gels D1/D2 :

### 3.1 Budget mémoire réel

Le buffer candidat H3d2 vaut 51 818 octets et est statique dans la composition STM32. Le cross-build produit déjà un fichier `.map`, mais aucun gel H3d3 n'établit encore un bilan final mesuré de :

- FLASH utilisée ;
- `.data` ;
- `.bss` ;
- réserve stack/heap du linker ;
- marge SRAM restante ;
- poids du buffer candidat dans cette marge.

D3 doit relever ces valeurs sur le binaire réel, sans modifier l'architecture pour les améliorer avant mesure.

### 3.2 Mesures temporelles physiques

Le cadrage H3d3-0 demandait :

- durée lecture 64 B ;
- durée écriture 64 B ;
- durée lecture 51 818 B ;
- durée écriture 51 818 B ;
- durée d'un commit H3d2 complet ;
- comportement/durée des timeouts.

Les campagnes D1/D2 ont prouvé la correction fonctionnelle mais n'ont pas gelé ces mesures.

D3 doit ajouter une instrumentation de qualification minimale utilisant une source de temps déjà disponible côté STM32. Les mesures sont descriptives : elles ne deviennent pas des exigences de performance sans seuil préalablement défini.

### 3.3 Alternance images/publications

Les observations physiques ont déjà exercé A et B et plusieurs générations, mais la campagne n'a pas été conçue comme un test autonome d'alternance répétée.

D3 doit vérifier, de manière non destructive au sens du format, une petite séquence bornée de commits normaux depuis l'autorité finale connue `VALID / gen4 / B / 0xA6`, avec observation des générations et images successives. Aucun grand test d'endurance n'est demandé.

## 4. Découpage proposé

### D3-A — budget mémoire et artefacts

Aucune modification fonctionnelle.

À partir du binaire courant :

1. relever `arm-none-eabi-size` ;
2. analyser le `.map` ;
3. établir FLASH/RAM utilisées et marge ;
4. identifier explicitement les 51 818 octets du candidate ;
5. documenter les réserves stack/heap définies par le linker.

Critère de sortie : bilan chiffré reproductible, sans conclusion de stack high-water non mesurée.

### D3-B — instrumentation temporelle bornée

Ajouter derrière une porte de qualification désarmée par défaut des sondes pour mesurer sur la FRAM réelle :

- read 64 B ;
- write 64 B sur une zone de qualification sûre ;
- read 51 818 B ;
- write 51 818 B ;
- commit H3d2 complet.

La zone et l'état de départ doivent être explicitement vérifiés avant toute écriture. Aucun retry automatique.

Critère de sortie : valeurs observées et méthode documentée.

### D3-C — alternance bornée

Depuis une baseline explicitement récupérée, effectuer un nombre faible et fixé de commits normaux, chacun avec une mutation reconnaissable, puis observer :

- génération monotone ;
- alternance image A/B ;
- alternance superblock A/B si instrumentée ;
- readback de la dernière valeur ;
- recovery après reset normal.

Critère de sortie : alternance observée sans formatage ni mécanisme de réparation.

## 5. Hors périmètre D3

D3 ne qualifie pas :

- brown-out/rampes d'alimentation ;
- coupure arbitraire au milieu d'une transaction SPI ;
- analyse électrique du bus pendant perte d'alimentation ;
- endurance FRAM ;
- campagne de fault injection SPI ;
- stack high-water si aucun mécanisme fiable n'est introduit ;
- performances de production avec acquisition vibration active ;
- DMA ou optimisation SPI.

Ces sujets restent backlog ou tranches dédiées. Ils ne bloquent pas la clôture de la persistance physique telle que cadrée ici.

## 6. Point d'architecture important

La composition STM32 actuelle reste principalement un harnais de bring-up/qualification et ne constitue pas encore le `SystemRuntime` STM32 complet.

Après clôture de D3, la suite de P12-H3 devra revenir aux dépendances de composition encore ouvertes, notamment :

- `WallClock` ;
- `TimeContinuityEvidenceProvider` ;
- `VibrationSource` ;
- valeurs de production de `ConfigurationValidationEnvironment` ;
- composition finale `SystemRuntime -> ModbusPduServerContext -> ModbusRtuServerRuntime`.

La qualification FRAM ne doit pas être confondue avec la fin de l'intégration STM32 globale.

## 7. Décision de progression

Commencer par **D3-A**, car il exploite uniquement les artefacts déjà produits, ne modifie ni firmware ni FRAM et peut révéler immédiatement une contrainte de ressources avant toute instrumentation supplémentaire.

D3-B et D3-C ne seront préparés qu'après revue du résultat D3-A.
