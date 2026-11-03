---
mission_id: TR2-20261103-SHIELD-V13-J5-FEMELLE-BOM
status: READY
base_ref: main
base_sha: e8cc6cc7cd895a62d5891b4e9410f0164abe9392
created_at_utc: 2026-11-03T16:51:03.218Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# MSM TR2 — Shield V1.3 : J5 femelle sous PCB, BOM économique, précommande

## Mission et décisions opérateur

**Mission technique exécutable immédiatement.** Le 03/11/2026, l'utilisateur confirme que **ses deux NUCLEO-U575ZI-Q C05 possèdent déjà deux contacts mâles soudés en CN12-69/70**. Ils resteront en place. Adapter **J5 du shield en réceptacle femelle à deux contacts sur la face inférieure**. La solution V1.2 « J5 mâle sur shield et femelle à souder sur Nucleo » est périmée **pour ces deux exemplaires**. Garder les autres connecteurs Zio et l'architecture inchangés.

L'utilisateur confirme aussi que **des connecteurs de gamme onéreuse ne sont pas nécessaires** pour deux prototypes assemblés manuellement, et souhaite une BOM réellement économique, tout en préservant stabilité de contact et fixation mécanique positive. Cible indicative : environ 50 EUR pour deux shields connectés, avec réemploi des breakouts ; coût réel à expliciter sans inventer devis, taux de change ni prix livré.

**Décisions de référence (lues sur `main`)** : `Modbus RTU/00_gouvernance/DECISIONS_PRECOMMANDE_SHIELD_TR2_20261103.md` et son *Avenant opérateur — J5 mâle déjà présent / achat économique*. La V1.2 `TR2-20261103-SHIELD-V12-PRECOMMANDE` est clôturée administrativement, statut technique `BLOCKED` conservé, archives `Echanges_Codex/archives/TR2-20261103-SHIELD-V12-PRECOMMANDE/`. Base CAO candidate dans `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/`, commit évalué `55cfcf2c0eea9a158117c47f5af84b0fc740fdf1`. Aucune modification rétroactive des anciens rapports.

## Travaux attendus

1. **Concevoir effectivement J5 femelle** : inspecter le PCB KiCad v1.2, symbole, footprint, pads, attribut d'assemblage face dessous et fichiers constructeurs. Choisir une embase femelle 1×2 **2,54 mm** de géométrie compatible avec **les broches mâles physiquement déjà soudées** sur chaque Nucleo aux positions CN12-69 et CN12-70. Attention à l'orientation/transposition des pads vue dessus/face inférieure : **PG4/CN12-69 doit rejoindre RTU_DE, PG6/CN12-70 reste NC**, sans inversion ou pont. Préférer une femelle traversante montée dessous si le fabricant l'autorise et si soudure/queues/boîtier restent dégagés ; considérer une alternative SMD ou autre montage propre si elle apporte une compatibilité vérifiable, sans câble Dupont ni dessoudage Nucleo. **Ne pas supposer la hauteur/section des broches existantes** : la confirmation utilisateur porte sur « mâles soudés », pas sur les cotes. Étudier la compatibilité de cette femelle avec la hauteur des mâles J1–J4 et des femelles Zio de la Nucleo, sans flexion ou insertion partielle. Conserver les trois entretoises/ancrages sans les faire forcer ; si une cote manque, la signaler précisément et finaliser toute la CAO indépendante.
2. **Optimiser les autres connecteurs sans faux luxe** : comparer les références Harwin V1.2 (J1–J4 mâles, J6/J7 1×9 femelles, J8 deux 1×12, J9 deux 1×8, J5 1×2 femelle) à des barrettes standards sécables / équivalents réellement disponibles. Vérifier **pas 2,54, taille des broches, section carrée, diamètres trous PCB, hauteur des sockets, longueur d'engagement, plage de températures adaptée au prototype, orientation dessous/dessus, possibilité de recouper proprement et compatibilité de soudure**. Préférer des références réellement approvisionnables, au coût total livré inférieur. Ni inférence de quantité/prix fantaisiste ni remplacement de composant de sûreté par friction seule ; les retenues mécaniques restent. Reporter une préférence fournisseur si prix/port réels inconnus et donner alternative chiffrée comme scénario, pas garantie.
3. **Conserver le reste** : SPI1 FRAM Adafruit4718, SPI2 SD Adafruit4682, SPI3 capteur MKIGIBV2/IIS3DWB, LPUART RS485 MIKROE3863/PG4 inchangés. Conserver microSD accessible côté -X, bornier RS485 vers +X avec GND2 isolé, nappe capteur vers -Y avec reprise d'effort, accès USER/LED/CN15 alimentation 5 V externe/ST-LINK. Aucune évolution firmware, aucun câblage de batterie ni boîtier final dans cette tranche.
4. **Adapter la CAO, pas seulement le texte** : correction minimale `TR2_Shield_Proto_V1.kicad_pcb`, empreinte J5 dessous et éventuellement autres empreintes réellement dépendantes des composants économiques choisis, tout en conservant le schéma fonctionnel et les nets. Mettre à jour le cartouche/sérigraphie de révision, symboles/footprints et scripts générateurs afin qu'une régénération ne restaure pas l'ancien J5 mâle ou les coûteuses références par accident. Mettre à jour BOM, budget **pour deux TR2 et lot de cinq PCB nus**, plan d'assemblage détaillé J5, gabarit imprimable 1:1, plan de retenue et liste d'achats différés. **Ne jamais libérer fabrication sur un J5 qui ne correspond pas au mâle réel**.
5. **Vérification indépendante des cotes essentielles** : l'inconnue C05 des centres Zio/ancrages n'est pas levée par le changement de connecteur. Fichiers de fabrication ST C05 restent à chercher raisonnablement si disponibles par canaux officiels ordinaires ; sans accès, conserver le gabarit papier et la dernière demande de contrôle physique sur **les deux** cartes. Pour J5, une photo nette dessus/de profil avec règle et éventuellement mesure du pas, longueur exposée/section/corps mâle permet de valider la femelle et hauteur d'engagement ; ne pas demander les mesures historiques MKI déjà acquises. Les fixations M2,5 avec très faible dégagement voisin des Zio doivent être examinées. Distinguer les limites PCB (précommande) des cotes adaptables après la réception (boîtier, guide LED, étriers).
6. **Qualification CAO et exports candidats** : relancer KiCad ERC, DRC strict et parité schéma/PCB, analyse netlist & cuivre/centre de perçages/gerber X2/Excellon, gabarit recto/verso avec repère J5 exact, documentation montage, contrôle que seuls J5/connecteurs justifiés sont modifiés. Corriger les défauts trouvés sans réduire les règles de sûreté. Si incertitude mécanique restante : publier les Gerber **NE PAS COMMANDER** dans zone protégée avec raison exacte et préciser ce que l'utilisateur peut vérifier en une intervention physique. Ne prétendre ni fit C05 démontré ni prix livré garanti.
7. **Publication** : commit/push normaux sur `main`, relecture distante effective des livrables et rapport sous `Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md`, avec `mission_id: TR2-20261103-SHIELD-V13-J5-FEMELLE-BOM`, statut fidèle (`DONE` uniquement si contrat entier démontré et vérifié ; sinon `BLOCKED` précis). Préserver toutes les archives, stashes et campagnes, modifier `ETAT_COURANT_TR2.md` uniquement factuellement. ChatGPT contrôlera indépendamment.

## Limites et autorisations

`target_branch: main`, `allow_commit: true`, `allow_push: true`, `allow_flash: false`, `allow_debug: false`. **Aucun flash, GDB, reset, accès microSD/FRAM, manipulation des deux Nucleo, achat, commande ou paiement.** L'utilisateur est en trajet et ne dispose pas encore d'un pied à coulisse ; ne pas le bloquer pour les travaux de CAO indépendants, ni lui transférer les opérations Git. Les gestes physiques, les mesures et la commande finale lui appartiennent. Ne pas faire réapparaître les détrompeurs anti-erreur complexes abandonnés pour les deux prototypes.

Cadre permanent : `AGENTS.md`, `Modbus RTU/00_gouvernance/GOUVERNANCE_CHATGPT_CODEX.md`, `Modbus RTU/00_gouvernance/Echanges_Codex/PROTOCOLE_ECHANGE_V2.md`, journal, décision précommande et rapport V1.2. Circuit obligatoire ChatGPT → GitHub → Codex → GitHub → ChatGPT.
