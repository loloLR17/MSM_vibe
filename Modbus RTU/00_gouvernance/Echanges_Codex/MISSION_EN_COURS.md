---
mission_id: TR2-20261104-SHIELD-V16-ETUDE-LED
status: READY
base_ref: main
base_sha: ee947e532fbd418cd787246d95f2237452198126
created_at_utc: 2026-11-04T13:15:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Shield TR2 — V1.6 : étude ciblée de visibilité de l'état boîtier fermé

## Besoin

L'utilisateur veut voir l'état du TR2 sans ouvrir le boîtier, simplement et à faible coût, sans négliger la robustesse. Préserver exactement les trois signaux actuels : LD1 verte PC7 (attente clignotante, fin fixe), LD2 bleue PB7 (initialisation/acquisition), LD3 rouge PG2 (erreur). Préférence pour les guides de lumière **s'ils sont simples** ; sinon solution de repli.

La V1.5 est clôturée **administrativement**, rapport original BLOCKED préservé. Lire AGENTS.md, gouvernance, protocole, la décision de clôture sous archives/TR2-20261104-SHIELD-V15-INTEGRATION-3D, puis les dossiers `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/`, notamment `docs/OPTIQUE_USER_V1_5.md`, `docs/DECISION_LIBERATION_PCB_V1_5.md`, `mechanical/V1_5/parameters.json` et le PCB KiCad V1.4. Les centres physiques LED C05 restent inconnus ; aucune découpe aveugle.

## Étude attendue

1. **Option optique** : évaluer trois guides courts/droits ou une petite fenêtre transparente, à travers une découpe localisée du shield si nécessaire. Mesurer sur la CAO disponible l'impact sur pistes, composants, supports, MKIGIBV2, boîtier et passage des LED LD1/2/3. Ne pas inventer de coordonnées C05. Exclure les fibres ou renvois compliqués si disproportionnés.
2. **Option électrique de repli** : analyser trois LED supplémentaires sur le shield, sans changer la signification des couleurs/clignotements. Vérifier **dans le schéma réel** la polarité, les résistances et courants des LED Nucleo, la disponibilité des signaux PC7/PB7/PG2 dans les connecteurs, et le besoin éventuel de buffers/résistances. Ne pas supposer que brancher une LED en parallèle est sûr ; vérifier conflits SPI/RS485 et occupation du PCB.
3. Comparer les options selon visibilité boîtier fermé, simplicité, montage manuel, coûts réels plausibles, résistance mécanique, absence d'ouverture non protégée, nombre de pièces, modifications nécessaires au PCB et risques firmware.
4. **Recommander une solution prioritaire et un seul repli**, avec justification et un croquis coté/vue de coupe sur le modèle existant, non un rendu esthétique non vérifié. Identifier précisément les **seules cotes C05 indispensables** si l'étude ne peut aboutir sans elles. Les photographies récentes LD1/2/3 et USER sont fournies dans le chat, pas nécessairement dans le dépôt accessible à Codex. Ne pas redemander les mesures acquises CN12 8,22/2,40 mm et MKI 15,2 mm.
5. **Livrer une note d'arbitrage concise** sous `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/docs/`, un plan de modification minimal si le PCB doit évoluer, et les preuves/sources. Préserver le fonctionnement USER mécanique et RESET protégé. Cette mission est **une étude seulement** : aucune modification du PCB, schéma KiCad, Gerber, firmware ou boîtier de fabrication ; toute correction structurelle sera décidée et mandatée séparément après revue ChatGPT.
6. Publier par commit/push sur main et rendre `DERNIER_RAPPORT.md` sous l'identifiant **TR2-20261104-SHIELD-V16-ETUDE-LED**, statut fidèle, preuves, SHA et relecture distante. Une conclusion conditionnelle est recevable si elle nomme ses vérifications physiques nécessaires ; ne pas annoncer une fabrication libérée.

## Permissions et sûreté

allow_commit=true, allow_push=true, allow_flash=false, allow_debug=false ; aucune intervention STM32, aucun achat, commande ou impression, aucune modification FRAM/microSD. Préserver historique, archives et acquis, appliquer A-06. Circuit ChatGPT → GitHub → Codex → GitHub → ChatGPT.
