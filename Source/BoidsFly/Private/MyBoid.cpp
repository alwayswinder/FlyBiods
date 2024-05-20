// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBoid.h"
#include "MyBoidsManager.h"
#include "Kismet\KismetSystemLibrary.h"


// Sets default values
AMyBoid::AMyBoid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ObjectTypesBird.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));
	ObjectTypesWall.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
}

// Called when the game starts or when spawned
void AMyBoid::BeginPlay()
{
	Super::BeginPlay();
	CurVelocity = GetActorForwardVector() * (SpeedMax + SpeedMin) / 2;
}

// Called every frame
void AMyBoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(BoidsManager)
	{
		if(!BoidsManager->UseCS)
		{
			UpdateBird();
		}
		else
		{
			PrimaryActorTick.bCanEverTick = false;
		}
	}
}

FVector AMyBoid::GetCurVelocity()
{
	return CurVelocity;
}

FVector AMyBoid::GetCurAcc()
{
	return  CurAcceleration;
}

bool AMyBoid::GetIsCollosion()
{
	return IsCollision;
}

void AMyBoid::UpdateBird()
{
	CurAcceleration = FVector(0, 0, 0);

	//聚合，同行，避让
	 if (GetRaysVectors())
	 {
	 	for (FVector RayVector : RaysVectors)
	 	{
	 		FHitResult Hit;
	 		FVector End = GetActorLocation() + RayVector * ViewRadius;
	 		UKismetSystemLibrary::SphereTraceSingleForObjects(this, GetActorLocation(), End, 5.0f,
	 			ObjectTypesWall, false, IgnoryActors, EDrawDebugTrace::None, Hit, true, FLinearColor::Green, FLinearColor::Red, 0.2f);
	 		if (!Hit.bBlockingHit)
	 		{
	 			CurAcceleration += RayVector * CollosionWeight;
	 			IsCollision = true;
	 			GetWorldTimerManager().SetTimer(CollisionTimer, this, &AMyBoid::SetIsCollosionFalse, LeaveTime, false, LeaveTime);
	 			//UE_LOG(LogTemp, Warning, TEXT("MeetCollosion"));
	 			break;
	 		}
	 	}
	 }

	if (IsCollision)
	{
		FHitResult Hit;
		UKismetSystemLibrary::SphereTraceSingleForObjects(this, GetActorLocation(),
			GetActorLocation()+FVector(0,0,1), AovRadius,
			ObjectTypesWall, false, IgnoryActors, EDrawDebugTrace::None, Hit, true);
		if (Hit.bBlockingHit)
		{
			FVector OffSetCollosion = GetActorLocation() - Hit.ImpactPoint;
			CurAcceleration += OffSetCollosion * leaveWallWeight / (OffSetCollosion.Size() * OffSetCollosion.Size());
			UE_LOG(LogTemp, Warning, TEXT("leaveWall"));
		}
	}
	else
	{
		if (!BoidsManager->UseCS)
		{
		
			TArray<AMyBoid*> NearBoids;

			TArray<FHitResult> Hits;
			UKismetSystemLibrary::SphereTraceMultiForObjects(this, GetActorLocation(),
				GetActorLocation()+FVector(0,0,1), ViewRadius,
				ObjectTypesBird, false, IgnoryActors, EDrawDebugTrace::None, Hits, true);

			for (FHitResult hit : Hits)
			{
				if (hit.bBlockingHit)
				{
					AMyBoid* Bird = Cast<AMyBoid>(hit.GetActor());
					if (Bird && !Bird->GetIsCollosion())
					{
						NearBoids.Add(Bird);
					}
				}
			}
			
			if (NearBoids.Num() > 0 && !IsCollision)
			{
				FVector Center = FVector(0, 0, 0);
				Aov = Aov * FreeWeight;
				FVector Flow = FVector(0, 0, 0);
				int BoidNum = 0;

				for (AMyBoid* Bird : NearBoids)
				{
					FVector OffsetVector = Bird->GetActorLocation() - GetActorLocation();
					float Distence = OffsetVector.Size();

					if (Distence <= ViewRadius)
					{
						Center += Bird->GetActorLocation();
						Flow += Bird->GetCurVelocity();
						BoidNum++;

						if (Distence <= AovRadius)
						{
							Aov -= OffsetVector / (Distence * Distence);
						}
					}
				}
				if (BoidNum > 0)
				{
					FVector GoalDirection = BoidsManager->GlobalDirection;
					if(BoidNum > BoidsManager->MaxGroupNum)
					{
						BoidsManager->MaxGroupNum = BoidNum;
						BoidsManager->GroupTarget = Center / BoidNum;
					}
					
					if(BoidNum <= MinGroupNum && BoidsManager->MaxGroupNum >= MinGroupNum + 5)
					{
						GoalDirection = BoidsManager->GroupTarget - GetActorLocation();
					}
					
					CurAcceleration += (Center / BoidNum - GetActorLocation()) * CenterWeight;
					CurAcceleration += (Flow + GoalDirection) / (float)BoidNum * FlowWeight;
					CurAcceleration += Aov * AovWeight;
				}
			}
			else
			{
				if(BoidsManager->MaxGroupNum >= MinGroupNum + 5)
				{
					CurAcceleration += BoidsManager->GroupTarget - GetActorLocation();
				}
			}
		}
		else
		{
			if(BoidsManager->BoidInfoSave.BoidInfo.IsValidIndex(BirdId))
			{
				CurAcceleration += FVector(BoidsManager->BoidInfoSave.BoidInfo[BirdId].Acceleration);
			}
		}
	}
	CurAcceleration = CurAcceleration.GetSafeNormal(0.0001f) * FMath::Clamp(CurAcceleration.Size(), 0.0f, 1.0f);
	CurVelocity += CurAcceleration;
	CurVelocity = CurVelocity.GetSafeNormal(0.0001f) * FMath::Clamp(CurVelocity.Size(), SpeedMin, SpeedMax);
	FVector NewLoc = GetActorLocation() + CurVelocity;
	NewLoc = ClampPos(NewLoc);
	SetActorLocation(NewLoc, true);
	SetActorRotation(FRotationMatrix::MakeFromX(CurVelocity.GetSafeNormal(0.0001f)).Rotator());
}

void AMyBoid::AddSelfToManage(AMyBoidsManager* InBoidsManager)
{
	if(InBoidsManager)
	{
		BoidsManager = InBoidsManager;
		BoidsManager->BoidInfoSave.AovRadius = AovRadius;
		BoidsManager->BoidInfoSave.ViewRadius = ViewRadius;
		BoidsManager->BoidInfoSave.BoidInfo[BirdId] = FMyBoidInfo(FVector3f(GetActorLocation()), FVector3f(CurVelocity));
		BoidsManager->BoidInfoSave.BoidRef[BirdId] = this;
	}
}

bool AMyBoid::GetRaysVectors()
{
	FHitResult Hit;
	FVector End = GetActorLocation() + GetActorForwardVector() * ViewRadius;
	UKismetSystemLibrary::SphereTraceSingleForObjects(this, GetActorLocation(), End, 5.0f,
		ObjectTypesWall, false, IgnoryActors, EDrawDebugTrace::None, Hit, true);
	if (Hit.bBlockingHit)
	{
		RaysVectors.Empty();
		for (int i=1; i <= CollosionRayNum; i++)
		{
			RaysVectors.Add((GetActorRightVector() - GetActorForwardVector()) * (float(i) / float(CollosionRayNum)) + GetActorForwardVector());
			RaysVectors.Add((GetActorRightVector() * -1.0f - GetActorForwardVector()) * (float(i) / float(CollosionRayNum)) + GetActorForwardVector());
			RaysVectors.Add((GetActorUpVector() - GetActorForwardVector()) * (float(i) / float(CollosionRayNum)) + GetActorForwardVector());
			RaysVectors.Add((GetActorUpVector() * -1.0f - GetActorForwardVector()) * (float(i) / float(CollosionRayNum)) + GetActorForwardVector());
		}
		return true;
	}
	return false;
}

void AMyBoid::SetIsCollosionFalse()
{
	IsCollision = false;
}

FVector AMyBoid::ClampPos(FVector Pos)
{
	if (FMath::Abs(SpawnLocation.X - Pos.X) > MaxActiveAreaSize ||
		FMath::Abs(SpawnLocation.Y - Pos.Y) > MaxActiveAreaSize ||
		FMath::Abs(SpawnLocation.Z - Pos.Z) > MaxActiveAreaSize)
	{
		return SpawnLocation;
	}
	return Pos;
}
