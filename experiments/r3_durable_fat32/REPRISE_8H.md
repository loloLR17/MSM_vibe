# Reprise R3-8H — même mission, aucune nouvelle acquisition

Mission `TR2-20261030-R3-8H-FAT32-AUTONOME`, état IN_PROGRESS après lancement.
Lire main courant/AGENTS/contrat/rapport et préserver tousworktrees/stashes.
Reprise locale : `/home/lolo/dev/msm/projects/MSM_vibe-R3-8h`.
Preuves/helpers : `/home/lolo/dev/msm/reports/TR2-r3-8h-autonomous`.
Firmware testé **1d3a5763c34649d36b216b8ab1a1b894ae0a1210**, BIN54536B
SHA`7e9e9b331486c116770e1aeaa48eb0f34025c19f0a47b136f4c453bcbe51b3af`.
Cible uniquementSN0032001F3235510C37333439/UID001B0046/38425018/20333157.
Ne pas flasher/reset/relancer quoi que ce soit. Carte reste dansTR2-002, alimenté
jusqu'à conservation du digestproducteurRAM et étatSDquiet réellement observé.

Le checkpoint de lancement donne PID/chemin natifWindows/session/heureannotée.
Témoin passif : `%TEMP%\TR2-R3-8H-20261030-e8-long\{ready.json,queries.jsonl,summary.json}`.
Il fonctionne sansCodex/WSL/GDB/réseau. Aucun job/surveillanceCodex8h àlaisser actif.
STOP/flush autonomes MCU à30600s nominales ; QPC doit démontrer>=28801s intérieur.
HeureUTC estimée n'est pas une preuve. En absence/échec du témoin, conserver
l'enregistrement et la réserve temporelle ; **aucune répétition automatique**.

## Après durée et retour de Codex, dans cet ordre

1. `python3 /home/lolo/dev/msm/reports/TR2-r3-8h-autonomous/read_native_after_run.py`
   Lit seulement les preuves existantes, vérifie UID/CRC/session/séquence/source et
   borneQPC puis terminalflags1284. Si fichier absent/erreur, **pas de retrait** ;
   examiner journal/processus et condition réelle avant toute action perturbatrice.
2. Terminal réellement conforme seulement :
   `python3 /home/lolo/dev/msm/reports/TR2-r3-8h-autonomous/collect_after_terminal.py e8-long-data 5 e8-long`
   GDB qualifié/UIDgardé, stage4/error0/clock_fault0/FIFOoff/writeroff/queue0/CShaute,
   dumpsRAM/histogrammes/SHAproducteur/header/footer/prefixes puisdetach. Ce digest
   indépendant du writer doit être sauvegardé **avant** coupure physique !
3. `python3 /home/lolo/dev/msm/reports/TR2-r3-8h-autonomous/e8_metrics.py`
   Contrôler compte, mémoire/pile, marges work/service/gap, clock/residual,
   FIFO etSDgrappes, tousseuils inchangés. Pas de nouvelle validation ancienne.
4. Demander àl'utilisateur **alors seulement** coupureSTLINK002/transfertSDvers
   lecteurPC/CN15resteoff/TR2-001intouché. Attendre confirmationhumaine.
5. `python3 /home/lolo/dev/msm/reports/TR2-r3-8h-autonomous/extract_e8_pc.py`
   Découvertelecteur réel, copies exclusives E8N0003/5 (v3,131072/671088640B),
   SHAentiercarte=copie, tousCRC/index/temps/footer/session/count etchaqueoctetutile
   auSHAproducteurRAM. Dixhistoriques+TR2INFOre-hachés. Croiserexplicitement session
   QPC avec header/footerPC et acceptedRAM/footer, pas seulement code retour0.
   Si INCOMPLETE/INVALID, préserveroriginaux/préfixe etfaits, pasDONE8h.
6. `python3 /home/lolo/dev/msm/reports/TR2-r3-8h-autonomous/package_e8.py`
   Publierseulement header+records/footer véritables avecEXTENTS/empreinteentière.
   **Jamais de zonepréallouée inutilisée ni de reconstructionqueuezéro commeoriginal.**
   Chunker au besoin grosartefacts (limitesGitHub), manifestes SHAavant/après ;
   pas de données privées/tokens. Code/helpersexacts etpreuvesbornées sous
   Echanges_Codex/preuves/TR2-20261030-R3-8H-FAT32-AUTONOME/.
7. Rapport/journal/commits/pushnormauxmain autorisés ; reliredepuisGitHubtous
   livrables etextents avantDONE, archivesexactesensuite, vérifierpublicationfinale.
   RevueA-01ChatGPT distincte. Pas de tag ni qualification V1/24h/FRAM/RTU/AC1/SI,
   ni de power-loss arbitraire déduits de la seule endurance isolée.

Helperspréparés pourreprise doivent être relus/ajustés aux faitsobservés, jamais
rejouésaveuglément ni utilisés pour prétendre que des étapes nonexécutées ontPASS.
Pas de restaurationautomatique : imageE8 bootWAIT conservée si saine, baseline24
etbackups restent disponibles. Si utilisateurabsent, checkpoint desfaits etattente
physique explicite, sans inventer un transfert de carte.
