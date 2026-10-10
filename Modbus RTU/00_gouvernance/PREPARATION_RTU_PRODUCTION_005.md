# Préparation runtime et qualification RS-485 — mission 005

10 octobre 2026 — `TR2-20261010-DEV-RTU-PRODUCTION-005`. Développement logiciel seulement. Aucune commande série, connexion, émission, flash ou session MCU exécutée. E4 microSD/power-loss reste ouverte. Ce document complète le plan 004 ; il ne change aucun freeze V1.

## Incrément de production livré

`ProductionApplication` possède durablement en RAM un `SystemRuntime`, les descripteurs de ses interfaces et son dispatcher B5. Il peut également posséder staging, workflow et adapter d’activation B4 : `production_application_enable_configuration` les initialise après boot contre son propre ConfigurationStore, avec un allocateur de métadonnées explicitement fourni. Les contextes matériels, buffers/backend de campagnes et capacités optionnelles restent empruntés explicitement. L'objet doit être statique ou vivre autant que l'application ; ne pas le copier/déplacer. Le boot est tenté une seule fois ; une erreur impose une récupération explicite, sans format ou retry automatique. Binding seulement après readiness ; adresse valide provisionnée et transport qualifié obligatoires.

`Stm32ProductionRuntime` possède ses propres FRAM, média transactionnel, candidat de 51 818 octets, source IIS3DWB et application. Il initialise les adapters existants à partir de SPI/CS et géométrie explicitement fournis, récupère la FRAM, refuse EMPTY/non-VALID, puis compose les horloges/reset/continuité et les vrais services. Il ne réutilise aucun objet local du harness. Le backend de campagnes est fourni séparément : ne pas substituer silencieusement un backend historique FRAM au backend microSD retenu. Préparer un backend SD durable et son allocation qualifiée reste nécessaire ; les fenêtres sacrificielles E3/E4 du harness ne sont pas un backend production.

`main.c` conserve l'appel existant à `stm32_modbus_application_bind`. Celui-ci possède maintenant un runtime statique et appelle `stm32_production_binding_acquire` avant toute préparation. Son implémentation faible refuse explicitement : aucun GPIO de direction ajouté, aucun boot média de production déclenché, firmware toujours en diagnostic RX. Une future implémentation de carte devra fournir transport qualifié, adresse/identité, backend campagnes et dépendances durables. La composition compilée ne constitue pas un essai sur cible. Ne pas activer ce port tant que le harness E1/E4 et le boot production peuvent se partager des autorités sans arbitrage documenté.

## B5 et limites explicites

Le dispatcher effectue lookup avant redispatch : retry même identité sans effet répété ; collision sans redispatch (erreur d'infrastructure donnant 04 dans ce binding, pas un nouveau résultat métier V1). Nouvelle identité valide : réservation durable avant validation métier, refus journalisé puis restitution idempotente. Échecs de réservation, STARTED ou COMPLETED remontés ; aucun succès final fabriqué.

- ID 0 : écriture FC16 valide, aucun effet ni identité persistante, statut refusé et code 14 exposés dans la vue courante ; `last` durable conservé. Code nul avec ID 0 suit aussi cette règle.
- Annulation seule ou combinée : bit consommé une fois, résultat 15 ; moteur/transaction métier et historique conservés. La politique V1 existante ne supporte aucune annulation réussie et bit9 reste nul.
- Codes 1/2 : exécuteurs existants raccordés au journal borné via nouvelles entrées `*_execute_bound` ; wrappers historiques conservés. APPLY requiert le workflow/port d'activation réel et ses métadonnées explicites ; validation CRC et payload avant apply ; CRC invalide → code 4, absence de préparé → 20, acquisition active → 5. SYNC utilise l'heure préparée B2 et la source centrale Modbus = 1, sans détourner param1 en source. Politique de paramètres existante inchangée.
- Codes 3/4 et 5..10 : dispatch vers les chemins runtime existants. SELFTEST et RESET exigent leurs capacités de plateforme réelles. Sans capacité, exception 04 avant réservation ; aucun simulacre d'autotest/reset. Code 11 : autorité de statistiques non disponible, exception 04 avant réservation. Aucune suppression de campagnes/configuration/histoire.
- Les champs courants peuvent restituer un ancien résultat rejoué ; `last` reste la dernière terminaison durable, pas la dernière requête réseau. Les refus ID0/concurrence/annulation sont des vues transitoires et ne créent pas de fausse histoire d'idempotence.

## Identification : rectificatif reçu pendant la mission

L’inventaire initial « ADM2587E / MIKROE-3863 » contredisait le [fabricant MIKROE](https://www.mikroe.com/rs485-isolator-2-click), qui associe **MIKROE-3863 au RS485 Isolator 2 Click / ADM2867E**. Après signalement, le rectificatif utilisateur publié dans le journal au SHA `463d69a8a6a2a36fe8a88f2905c6cad3ff5c3fac` confirme **ADM2867E / MIKROE-3863 sur le TR2 actuel, deux ADM2867E au total et un ADM2587E séparé sans carte support identifiée**. Il remplace l’inventaire initial sans réécrire la mission READY. L’écart d’identité est levé par cette déclaration, pas par une mesure Codex ; marquage U1, révision et câblage devront néanmoins être contrôlés lors du jalon électrique.

Sources constructeur consultées le 10 octobre 2026 : [ADM2587E Rev H](https://www.analog.com/media/en/technical-documentation/data-sheets/adm2582e-2587e.pdf), [ADM2867E](https://www.analog.com/en/products/adm2867e.html), [schéma Click v102](https://download.mikroe.com/documents/add-on-boards/click/rs485_lsolator_2_click/rs485-isolator-2-click-schematic-v102.pdf). Le schéma porte U1 ADM2867EBRNZ. Ce schéma n'est applicable au banc qu'après identification de sa révision.

ADM2587E : 20 broches, 500 kbit/s, alimentation 3,3/5 V et isolation signal/puissance 2,5 kVrms ; DE actif haut, /RE actif bas. Sa logique dépend de VCC : seuil haut 0,7×VCC et sortie RX proche de VCC ; alimenter en 5 V ne démontre donc pas une interface sûre et fonctionnelle vers des GPIO 3,3 V. ADM2867E : 28 broches, 25 Mbit/s, isolation 5,7 kVrms, VCC 3..5,5 V et VIO distinct 1,7..5,5 V. Ces caractéristiques de composants ne qualifient ni les niveaux ni l'isolation du montage assemblé. Ils ne sont pas interchangeables par brochage.

## Matrice des preuves et contrôles futurs

« Prouvé » signifie ici lecture du code/document constructeur, sauf indication explicite. Aucune mesure électrique nouvelle.

| Élément | Prouvé documentaire/code | À vérifier/mesurer sur le banc | Inconnu actuel |
|---|---|---|---|
| Identité | MIKROE-3863 constructeur = ADM2867E | Marquage U1, modèle/révision et photos | Révision réelle et carte support du seul ADM2587E |
| Alimentation | Click : sélection VCC SEL 3,3/5 V ; schéma v102 distingue VIO et VCC | Position réelle, liaison effective de VIO, tension VCC/VIO, capacité de la source | Rails effectivement raccordés |
| Logique UART | PG7 TX / PG8 RX configurés LPUART1, 115200 / 8E1 | Niveaux, continuité TXD/RXD et référence logique | Niveaux effectifs et continuité du montage |
| Pinout | Click : /RE sur RST, DE sur CS ; schéma nomme RXD/TXD, INVD/INVR | Tracer les nets jusqu'à U1 ; labels RX/TX ne suffisent pas | Câblage réel et inversion |
| Direction | PG5 CN12-68 / PG4 CN12-69 seulement candidats dans la conception | Choix signé, niveaux de repos, fin physique TX avant RX | Affectation et stratégie d'écho |
| VDDIO2 | PG4..PG8 concernés dans la conception ; pas d'EnableVddIO2 explicite dans le transport | Alimentation domaine, état d'isolation et configuration MCU/NUCLEO | Niveau et disponibilité réels |
| Duplex | Click schéma : J7/J8 half-duplex ; J2/J3 sélection contrôle | État réel des ponts, association paire RX A/B et TX Y/Z | Mode réel 2/4 fils |
| Isolation/masses | Schéma distingue GND logique et GND2 bus | Tracer masses/câble/terre ; vérifier absence de pont parasite | Isolation effective de l'ensemble |
| Terminaison | Schéma : J5/J9 sélection Term | Résistances et position aux seules extrémités du bus | Terminaisons du câble et des cartes |
| Polarisation | Schéma : J4/J6 Bias | Population/valeurs, tension différentielle au repos | Polarisation du câble, cumul sur le réseau |
| A/B, Y/Z | Noms constructeur présents ; pas de correspondance universelle avec +/− du câble | Identifier bornes fabricant et sens par mesure après autorisation | Polarité et éventuelle inversion câble |
| Câble USB | Reçu, jamais branché selon utilisateur | USB VID/PID, modèle, pilote, COM, bornes, direction automatique | Fonctionnement, isolation, limites électriques |

Décision actuelle : les prérequis de direction ne sont pas établis. Aucun adapter GPIO DE//RE n'est activé ni testé sur matériel ; ajouter une logique avec des broches supposées masquerait ce blocage. Un futur transport doit attendre la fin **physique** du dernier stop bit, remettre le bus en réception et définir le traitement de l'écho, en conservant les frontières T1.5/T3.5. Les 750/1750 µs du firmware restent des valeurs nominales, non des mesures du bus.

Références projet relues : `CONCEPTION_FIRMWARE_P12H3H_B2_NUCLEO_PHYSICAL_OCCUPANCY.md`, `FREEZE_FIRMWARE_P12F_STM32_LPUART1.md`, `FREEZE_FIRMWARE_P12G_STM32_RTU_TIMING.md`, plan 004, transport STM32 courant. Les documents historiques EVAL-ADM2587EARDZ ne sont pas un schéma de la carte Click réelle.

## Jalon A futur — câble sur PC seul

**À exécuter sous une mission ultérieure autorisée ; rien exécuté en 005.** TR2 et fils RS-485 restent déconnectés. L'utilisateur relève modèle/étiquette/notice du câble, puis branche seulement son USB au PC lorsqu'il y est invité. Tester l'énumération Windows avant de traiter une éventuelle exposition WSL.

Dans Windows PowerShell 5.1, avant puis après branchement :

```powershell
Get-PnpDevice -PresentOnly | Select-Object Status,Class,FriendlyName,InstanceId
[System.IO.Ports.SerialPort]::GetPortNames()
Get-CimInstance Win32_SerialPort | Select-Object DeviceID,Name,PNPDeviceID
```

Relever la nouvelle identité VID/PID, le port et l'état du pilote dans le Gestionnaire de périphériques. Un port COM n'est pas une preuve de transceiver fonctionnel ; si aucun port apparaît, rechercher le pilote officiel du fabricant identifié, sans en choisir un au hasard. Ne pas confondre le COM ST-LINK et le câble. Commandes fondées sur [Get-PnpDevice](https://learn.microsoft.com/en-us/powershell/module/pnpdevice/get-pnpdevice) et [SerialPort](https://learn.microsoft.com/en-us/dotnet/api/system.io.ports.serialport).

Après identification du COM, test d'ouverture/fermeture **sans envoyer d'octet**, toujours PC seul :

```powershell
$tr2CablePort = Read-Host 'Port du câble identifié (ex. COM7)'
if ([System.IO.Ports.SerialPort]::GetPortNames() -notcontains $tr2CablePort) {
    throw 'Port non énuméré'
}
$tr2Cable = [System.IO.Ports.SerialPort]::new(
    $tr2CablePort, 115200, [System.IO.Ports.Parity]::Even,
    8, [System.IO.Ports.StopBits]::One)
$tr2Cable.Handshake = [System.IO.Ports.Handshake]::None
$tr2Cable.DtrEnable = $false
$tr2Cable.RtsEnable = $false
try {
    $tr2Cable.Open()
    $tr2Cable | Select-Object PortName,BaudRate,DataBits,Parity,StopBits,IsOpen
} finally {
    if ($tr2Cable.IsOpen) { $tr2Cable.Close() }
    $tr2Cable.Dispose()
}
```

Critère A : modèle et port identifiés, pilote sans erreur, ouverture 115200/8E1 réussie et fermeture confirmée. Pas de validation d'émission/réception RS-485, pas de boucle A/B improvisée. Le passage USB vers WSL, s'il est nécessaire, constitue une étape distincte après identification, sans modification permanente du poste dans cette mission.

## Jalon B futur — vérification électrique puis un TR2

Avant tout raccordement, contrôler référence/révision selon le rectificatif reçu et compléter chaque case inconnue de la matrice, hors tension pour les contrôles de continuité/jumpers. L'utilisateur réalise les gestes ; une mission physique distincte autorise les opérations. Consigner les tensions/niveaux, choix de masse bus et logique, terminaison/polarisation, duplex, bornes câble et schéma final ; aucun branchement ne découle automatiquement de ce document.

Puis qualifier les autorités de production (backend SD/allocation, FRAM provisionnée sans format implicite, adresse et B0, DE//RE/VDDIO2), arrêter toute cohabitation non arbitrée avec les écritures du harness. Build attendu et procédure flash/debug qualifiés sous autorisations dédiées. En premier : lecture B1 unitaire, CRC et adressage, absence de réponse mauvaise adresse/CRC, fonction non supportée, puis timing TX/RX/écho/turnaround mesuré. L'outil offline `tr2_rtu_request` et le plan 004 servent à préparer les trames ; leurs commandes d'émission restent subordonnées à ce jalon et à la nouvelle mission.

## Jalon C futur — deux TR2 et supervision

Identifier séparément les deux cartes disponibles, assembler/qualifier chaque TR2 isolément, provisionner deux adresses uniques et vérifier deux identités B0 réelles. Constituer ensuite un bus linéaire avec terminaisons aux extrémités et polarisation décidée, en conservant les preuves A/B et masses. Tester lecture alternée des deux capteurs, corrélation indépendante des transaction_id/résultats, timeouts/déconnexion d'un seul nœud, reprise sans double effet et absence de collision. Enfin qualifier la supervision simultanée. Le second TR2 n'est pas présumé assemblé ou prêt ; aucune réussite multi-TR2 n'est revendiquée.
